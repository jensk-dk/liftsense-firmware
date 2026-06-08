#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

#include "imu.h"
#include "ble.h"
#include "sensor_math.h"
#include "imu_packet.h"

static struct imu_packet imu_data;
static uint32_t last_filter_time_ms;

int main(void)
{
    printk("LiftSense firmware booting...\n");

    /* Initialize IMU */
    if (imu_init() < 0) {
        return -1;
    }

    /* Initialize BLE */
    if (ble_init() < 0) {
        return -1;
    }

    /* Start BLE advertising */
    if (ble_start_advertising() < 0) {
        return -1;
    }

    /* Set up IMU trigger */
    if (imu_setup_trigger(NULL) < 0) {
        return -1;
    }

    /* Calibrate gravity vector */
    double gx, gy, gz;
    double gravity_magnitude = imu_calibrate_gravity(&gx, &gy, &gz);
    
    /* Initialize sensor math module */
    sensor_math_init(gx, gy, gz, gravity_magnitude);
    last_filter_time_ms = k_uptime_get_32();

    printk("LiftSense streaming started\n");
    printk("BLE Packet Format (18 bytes, little-endian):\n");
    printk("  [0-3]   timestamp_ms (uint32_t)\n");
    printk("  [4-5]   ax (int16_t, m/s² × 1000)\n");
    printk("  [6-7]   ay (int16_t, m/s² × 1000)\n");
    printk("  [8-9]   az (int16_t, m/s² × 1000)\n");
    printk("  [10-11] gx (int16_t, dps × 1000)\n");
    printk("  [12-13] gy (int16_t, dps × 1000)\n");
    printk("  [14-15] gz (int16_t, dps × 1000)\n");
    printk("  [16-17] vertical (int16_t, m/s² × 1000)\n");
    printk("\n");

    while (1) {
        struct sensor_value accel_x, accel_y, accel_z;
        struct sensor_value gyro_x, gyro_y, gyro_z;
        
        /* Get current sensor values */
        imu_get_values(&accel_x, &accel_y, &accel_z,
                      &gyro_x, &gyro_y, &gyro_z);
        
        double ax_d = sensor_value_to_double(&accel_x);
        double ay_d = sensor_value_to_double(&accel_y);
        double az_d = sensor_value_to_double(&accel_z);
        double gx_d = sensor_value_to_double(&gyro_x);
        double gy_d = sensor_value_to_double(&gyro_y);
        double gz_d = sensor_value_to_double(&gyro_z);
        
        /* Update complementary filter */
        uint32_t current_time_ms = k_uptime_get_32();
        double dt = (current_time_ms - last_filter_time_ms) / 1000.0;
        if (dt > 0.001 && dt < 1.0) {  /* Sanity check */
            sensor_math_update_filter(ax_d, ay_d, az_d, gx_d, gy_d, gz_d, dt);
        }
        last_filter_time_ms = current_time_ms;
        
        /* Pack IMU data for BLE transmission */
        imu_data.timestamp_ms = current_time_ms;
        imu_data.ax = sensor_to_milli_int16(&accel_x);
        imu_data.ay = sensor_to_milli_int16(&accel_y);
        imu_data.az = sensor_to_milli_int16(&accel_z);
        imu_data.gx = sensor_to_milli_int16(&gyro_x);
        imu_data.gy = sensor_to_milli_int16(&gyro_y);
        imu_data.gz = sensor_to_milli_int16(&gyro_z);
        
        /* Calculate vertical acceleration */
        double vertical_accel = sensor_math_calc_vertical(ax_d, ay_d, az_d);
        imu_data.vertical = (int16_t)(vertical_accel * 1000.0);

        /* Send BLE notification if enabled */
        if (ble_is_notify_enabled()) {
            ble_notify_imu_data(&imu_data);
        }

        /* Print to console for debugging */
        double grav_x, grav_y, grav_z;
        sensor_math_get_gravity_estimate(&grav_x, &grav_y, &grav_z);
        
        printf("CONSOLE: Accel: x=%.2f y=%.2f z=%.2f m/s² | "
               "Gyro: x=%.2f y=%.2f z=%.2f dps | "
               "Vert: %.2f m/s² | Grav: [%.2f,%.2f,%.2f]\n",
               ax_d, ay_d, az_d,
               gx_d, gy_d, gz_d,
               imu_data.vertical / 1000.0,
               grav_x, grav_y, grav_z);
        
        /* Also print as int16 millis to match BLE format */
        printk("PACKED:  ts=%u ax=%d ay=%d az=%d gx=%d gy=%d gz=%d vert=%d\n",
               imu_data.timestamp_ms,
               imu_data.ax, imu_data.ay, imu_data.az,
               imu_data.gx, imu_data.gy, imu_data.gz,
               imu_data.vertical);

        k_sleep(K_MSEC(20));  /* 50 Hz streaming rate */
    }
    
    return 0;
}
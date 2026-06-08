#include "imu.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <math.h>

static struct sensor_value accel_x, accel_y, accel_z;
static struct sensor_value gyro_x, gyro_y, gyro_z;
static const struct device *imu_dev;
static imu_data_callback_t user_callback;

static void imu_trigger_handler(const struct device *dev,
                                const struct sensor_trigger *trig)
{
    /* Fetch and get accelerometer data */
    sensor_sample_fetch_chan(dev, SENSOR_CHAN_ACCEL_XYZ);
    sensor_channel_get(dev, SENSOR_CHAN_ACCEL_X, &accel_x);
    sensor_channel_get(dev, SENSOR_CHAN_ACCEL_Y, &accel_y);
    sensor_channel_get(dev, SENSOR_CHAN_ACCEL_Z, &accel_z);

    /* Fetch and get gyroscope data */
    sensor_sample_fetch_chan(dev, SENSOR_CHAN_GYRO_XYZ);
    sensor_channel_get(dev, SENSOR_CHAN_GYRO_X, &gyro_x);
    sensor_channel_get(dev, SENSOR_CHAN_GYRO_Y, &gyro_y);
    sensor_channel_get(dev, SENSOR_CHAN_GYRO_Z, &gyro_z);
    
    /* Call user callback if registered */
    if (user_callback) {
        user_callback(&accel_x, &accel_y, &accel_z,
                     &gyro_x, &gyro_y, &gyro_z);
    }
}

int imu_init(void)
{
    imu_dev = DEVICE_DT_GET_ONE(st_lsm6dsl);

    if (!device_is_ready(imu_dev)) {
        printk("LSM6DSL device not ready\n");
        return -1;
    }

    /* Set sampling frequency to 104 Hz */
    struct sensor_value odr_attr = {.val1 = 104, .val2 = 0};

    if (sensor_attr_set(imu_dev, SENSOR_CHAN_ACCEL_XYZ,
                        SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
        printk("Cannot set accel sampling frequency\n");
        return -1;
    }

    if (sensor_attr_set(imu_dev, SENSOR_CHAN_GYRO_XYZ,
                        SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
        printk("Cannot set gyro sampling frequency\n");
        return -1;
    }

    printk("IMU initialized at 104 Hz\n");
    return 0;
}

int imu_setup_trigger(imu_data_callback_t callback)
{
    user_callback = callback;
    
    /* Set up data-ready trigger */
    struct sensor_trigger trig = {
        .type = SENSOR_TRIG_DATA_READY,
        .chan = SENSOR_CHAN_ACCEL_XYZ
    };

    if (sensor_trigger_set(imu_dev, &trig, imu_trigger_handler) != 0) {
        printk("Could not set trigger\n");
        return -1;
    }

    printk("IMU trigger configured\n");
    return 0;
}

double imu_calibrate_gravity(double *gx, double *gy, double *gz)
{
    printk("Calibrating gravity vector...\n");
    k_sleep(K_MSEC(500));  /* Let sensor stabilize */
    
    /* Average several samples for gravity calibration */
    double sum_x = 0, sum_y = 0, sum_z = 0;
    for (int i = 0; i < 10; i++) {
        sum_x += sensor_value_to_double(&accel_x);
        sum_y += sensor_value_to_double(&accel_y);
        sum_z += sensor_value_to_double(&accel_z);
        k_sleep(K_MSEC(20));
    }
    
    *gx = sum_x / 10.0;
    *gy = sum_y / 10.0;
    *gz = sum_z / 10.0;
    
    double magnitude = sqrt((*gx) * (*gx) + (*gy) * (*gy) + (*gz) * (*gz));
    
    /* Normalize to unit vector */
    *gx /= magnitude;
    *gy /= magnitude;
    *gz /= magnitude;
    
    printk("Gravity calibrated: [%.2f, %.2f, %.2f] (mag: %.2f m/s²)\n",
           *gx, *gy, *gz, magnitude);
    
    return magnitude;
}

void imu_get_values(struct sensor_value *ax,
                    struct sensor_value *ay,
                    struct sensor_value *az,
                    struct sensor_value *gx,
                    struct sensor_value *gy,
                    struct sensor_value *gz)
{
    if (ax) *ax = accel_x;
    if (ay) *ay = accel_y;
    if (az) *az = accel_z;
    if (gx) *gx = gyro_x;
    if (gy) *gy = gyro_y;
    if (gz) *gz = gyro_z;
}

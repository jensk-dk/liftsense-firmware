#ifndef IMU_H
#define IMU_H

#include <zephyr/drivers/sensor.h>

/* IMU sensor callback type */
typedef void (*imu_data_callback_t)(struct sensor_value *accel_x,
                                    struct sensor_value *accel_y,
                                    struct sensor_value *accel_z,
                                    struct sensor_value *gyro_x,
                                    struct sensor_value *gyro_y,
                                    struct sensor_value *gyro_z);

/* Initialize IMU sensor */
int imu_init(void);

/* Set up IMU trigger with callback */
int imu_setup_trigger(imu_data_callback_t callback);

/* Calibrate gravity vector - returns magnitude */
double imu_calibrate_gravity(double *gx, double *gy, double *gz);

/* Get current sensor values */
void imu_get_values(struct sensor_value *accel_x,
                    struct sensor_value *accel_y,
                    struct sensor_value *accel_z,
                    struct sensor_value *gyro_x,
                    struct sensor_value *gyro_y,
                    struct sensor_value *gyro_z);

#endif /* IMU_H */

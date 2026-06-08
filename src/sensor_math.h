#ifndef SENSOR_MATH_H
#define SENSOR_MATH_H

#include <zephyr/drivers/sensor.h>

/* Initialize sensor math module with initial gravity vector */
void sensor_math_init(double gx, double gy, double gz, double magnitude);

/* Update complementary filter with new sensor readings */
void sensor_math_update_filter(double ax, double ay, double az,
                               double gx, double gy, double gz,
                               double dt);

/* Calculate vertical acceleration relative to gravity */
double sensor_math_calc_vertical(double ax, double ay, double az);

/* Get current gravity estimate */
void sensor_math_get_gravity_estimate(double *gx, double *gy, double *gz);

/* Convert sensor_value to int16 millis for BLE transmission */
int16_t sensor_to_milli_int16(const struct sensor_value *v);

#endif /* SENSOR_MATH_H */

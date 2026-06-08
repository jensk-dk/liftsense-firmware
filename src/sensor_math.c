#include "sensor_math.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Gravity vector (normalized direction and magnitude) */
static double gravity_x, gravity_y, gravity_z;
static double gravity_magnitude;

/* Complementary filter - tracks current gravity direction in sensor frame */
static double grav_est_x, grav_est_y, grav_est_z;

void sensor_math_init(double gx, double gy, double gz, double magnitude)
{
    gravity_x = gx;
    gravity_y = gy;
    gravity_z = gz;
    gravity_magnitude = magnitude;
    
    /* Initialize gravity estimate */
    grav_est_x = gx;
    grav_est_y = gy;
    grav_est_z = gz;
}

void sensor_math_update_filter(double ax, double ay, double az,
                               double gx, double gy, double gz,
                               double dt)
{
    /* Filter coefficient (0.98 = trust gyro 98%, accel 2%) */
    const double alpha = 0.98;
    
    /* Convert gyro from deg/s to rad/s */
    double gx_rad = gx * M_PI / 180.0;
    double gy_rad = gy * M_PI / 180.0;
    double gz_rad = gz * M_PI / 180.0;
    
    /* Predict gravity direction by rotating current estimate with gyro */
    /* Small angle approximation: rotate gravity vector by -gyro * dt */
    double gx_dt = gx_rad * dt;
    double gy_dt = gy_rad * dt;
    double gz_dt = gz_rad * dt;
    
    /* Cross product: gravity_new = gravity - (gyro × gravity) * dt */
    double pred_x = grav_est_x - (gy_dt * grav_est_z - gz_dt * grav_est_y);
    double pred_y = grav_est_y - (gz_dt * grav_est_x - gx_dt * grav_est_z);
    double pred_z = grav_est_z - (gx_dt * grav_est_y - gy_dt * grav_est_x);
    
    /* Normalize prediction */
    double pred_mag = sqrt(pred_x * pred_x + pred_y * pred_y + pred_z * pred_z);
    if (pred_mag > 0.1) {
        pred_x /= pred_mag;
        pred_y /= pred_mag;
        pred_z /= pred_mag;
    }
    
    /* Correct with accelerometer (normalized) */
    double accel_mag = sqrt(ax * ax + ay * ay + az * az);
    double accel_weight = alpha;
    
    /* If total acceleration is close to 1g, trust accelerometer more */
    if (accel_mag > 8.0 && accel_mag < 12.0) {
        /* Near 1g - likely stationary or constant velocity */
        accel_weight = 0.95;  /* Trust accel more */
    }
    
    if (accel_mag > 0.1) {
        double accel_x_norm = ax / accel_mag;
        double accel_y_norm = ay / accel_mag;
        double accel_z_norm = az / accel_mag;
        
        /* Complementary filter: blend gyro prediction with accel correction */
        grav_est_x = accel_weight * pred_x + (1.0 - accel_weight) * accel_x_norm;
        grav_est_y = accel_weight * pred_y + (1.0 - accel_weight) * accel_y_norm;
        grav_est_z = accel_weight * pred_z + (1.0 - accel_weight) * accel_z_norm;
    } else {
        /* High acceleration - just use gyro prediction */
        grav_est_x = pred_x;
        grav_est_y = pred_y;
        grav_est_z = pred_z;
    }
    
    /* Normalize final estimate */
    double est_mag = sqrt(grav_est_x * grav_est_x + 
                         grav_est_y * grav_est_y + 
                         grav_est_z * grav_est_z);
    if (est_mag > 0.1) {
        grav_est_x /= est_mag;
        grav_est_y /= est_mag;
        grav_est_z /= est_mag;
    }
}

double sensor_math_calc_vertical(double ax, double ay, double az)
{
    /* Dot product gives total acceleration along gravity direction */
    double accel_along_gravity = (ax * grav_est_x + ay * grav_est_y + az * grav_est_z);
    
    /* Subtract gravity magnitude to get net vertical acceleration */
    return accel_along_gravity - gravity_magnitude;
}

void sensor_math_get_gravity_estimate(double *gx, double *gy, double *gz)
{
    if (gx) *gx = grav_est_x;
    if (gy) *gy = grav_est_y;
    if (gz) *gz = grav_est_z;
}

int16_t sensor_to_milli_int16(const struct sensor_value *v)
{
    return (int16_t)(v->val1 * 1000 + v->val2 / 1000);
}

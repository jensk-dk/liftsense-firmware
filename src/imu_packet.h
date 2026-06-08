#ifndef IMU_PACKET_H
#define IMU_PACKET_H

#include <stdint.h>

/* IMU data packet structure for BLE transmission */
struct __attribute__((packed)) imu_packet {
    uint32_t timestamp_ms;
    int16_t ax;  /* Accelerometer X (m/s² × 1000) */
    int16_t ay;  /* Accelerometer Y (m/s² × 1000) */
    int16_t az;  /* Accelerometer Z (m/s² × 1000) */
    int16_t gx;  /* Gyroscope X (dps × 1000) */
    int16_t gy;  /* Gyroscope Y (dps × 1000) */
    int16_t gz;  /* Gyroscope Z (dps × 1000) */
    int16_t vertical;  /* Vertical acceleration relative to gravity (m/s² × 1000) */
};

#endif /* IMU_PACKET_H */

#ifndef BLE_H
#define BLE_H

#include "imu_packet.h"
#include <stdbool.h>

/* Initialize BLE stack */
int ble_init(void);

/* Start BLE advertising */
int ble_start_advertising(void);

/* Send IMU data notification to connected client */
int ble_notify_imu_data(const struct imu_packet *data);

/* Check if BLE notifications are enabled */
bool ble_is_notify_enabled(void);

/* Update battery level (0-100%) for Battery Service */
int ble_update_battery_level(uint8_t level);

#endif /* BLE_H */

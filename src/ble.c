#include "ble.h"
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>
#include <stddef.h>

static struct imu_packet imu_data;
static bool notify_enabled;

/* BLE Service UUIDs */
/* LiftSense IMU Service: a0f1f001-5b25-4f91-a8e3-2c6d9f7b3e4a */
#define BT_UUID_IMU_SERVICE_VAL \
    BT_UUID_128_ENCODE(0xa0f1f001, 0x5b25, 0x4f91, 0xa8e3, 0x2c6d9f7b3e4a)

/* LiftSense IMU Data Characteristic: a0f1f002-5b25-4f91-a8e3-2c6d9f7b3e4a */
#define BT_UUID_IMU_CHAR_VAL \
    BT_UUID_128_ENCODE(0xa0f1f002, 0x5b25, 0x4f91, 0xa8e3, 0x2c6d9f7b3e4a)

static struct bt_uuid_128 imu_service_uuid = BT_UUID_INIT_128(BT_UUID_IMU_SERVICE_VAL);
static struct bt_uuid_128 imu_char_uuid = BT_UUID_INIT_128(BT_UUID_IMU_CHAR_VAL);

/* BLE notification callback */
static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    printk("BLE notifications %s\n", notify_enabled ? "enabled" : "disabled");
}

/* BLE GATT Service Definition */
BT_GATT_SERVICE_DEFINE(imu_svc,
    BT_GATT_PRIMARY_SERVICE(&imu_service_uuid),
    BT_GATT_CHARACTERISTIC(&imu_char_uuid.uuid,
                          BT_GATT_CHRC_NOTIFY,
                          BT_GATT_PERM_NONE,
                          NULL, NULL, &imu_data),
    BT_GATT_CCC(ccc_cfg_changed,
               BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
);

/* BLE Advertisement Data */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

int ble_init(void)
{
    int err = bt_enable(NULL);
    if (err) {
        printk("BLE init failed (err %d)\n", err);
        return err;
    }

    printk("BLE initialized\n");
    return 0;
}

int ble_start_advertising(void)
{
    struct bt_le_adv_param adv_param = BT_LE_ADV_PARAM_INIT(
        BT_LE_ADV_OPT_CONN,
        BT_GAP_ADV_FAST_INT_MIN_1,
        BT_GAP_ADV_FAST_INT_MAX_1,
        NULL);
    
    int err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        return err;
    }

    printk("BLE advertising started (LiftSense)\n");
    return 0;
}

int ble_notify_imu_data(const struct imu_packet *data)
{
    if (!notify_enabled) {
        return -1;  /* Not enabled */
    }
    
    /* Debug: Print BLE packet contents */
    printk("BLE TX: ts=%u ax=%d ay=%d az=%d gx=%d gy=%d gz=%d vert=%d\n",
           data->timestamp_ms,
           data->ax, data->ay, data->az,
           data->gx, data->gy, data->gz,
           data->vertical);
    
    /* Debug: Print raw bytes (little-endian on nRF52) */
    const uint8_t *bytes = (const uint8_t *)data;
    printk("BLE RAW [%zu bytes]: ", sizeof(*data));
    for (size_t i = 0; i < sizeof(*data); i++) {
        printk("%02x ", bytes[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9 || i == 11 || i == 13 || i == 15) {
            printk("| ");  /* Field separator */
        }
    }
    printk("\n");
    
    return bt_gatt_notify(NULL, &imu_svc.attrs[1], data, sizeof(*data));
}

bool ble_is_notify_enabled(void)
{
    return notify_enabled;
}

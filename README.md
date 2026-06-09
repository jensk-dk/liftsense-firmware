# LiftSense Firmware

Zephyr-based firmware for LiftSense IMU + BLE barbell velocity sensor.

## Overview

This project runs on the Seeed XIAO nRF52840 Sense and streams IMU data over BLE for velocity-based training (VBT) applications.

**Features:**
- Real-time 6-axis IMU sampling at 104 Hz (LSM6DS3 accelerometer + gyroscope)
- BLE GATT service for wireless data streaming at 50 Hz
- Complementary filter for rotation-invariant vertical acceleration tracking
- Automatic gravity calibration at startup
- Low-latency interrupt-driven sensor reading

---

## Implementation Status

**Completed:**
- Serial console logging
- IMU sensor initialization and sampling (104 Hz)
- BLE peripheral advertising and GATT service
- BLE streaming of IMU data (50 Hz)
- Gravity calibration and complementary filter
- Rotation-invariant vertical acceleration calculation
- Battery monitoring and BLE Battery Service
- Charging state detection

---

## BLE Service

### LiftSense IMU Service
**Service UUID:** `a0f1f001-5b25-4f91-a8e3-2c6d9f7b3e4a`

#### IMU Data Characteristic
**UUID:** `a0f1f002-5b25-4f91-a8e3-2c6d9f7b3e4a`  
**Properties:** Notify  
**Update Rate:** 50 Hz (20ms intervals)

### Standard Battery Service
**Service UUID:** `0x180F` (Standard BLE Battery Service)

#### Battery Level Characteristic
**UUID:** `0x2A19` (Standard BLE Battery Level)  
**Properties:** Read, Notify  
**Update Rate:** Every 30 seconds  
**Value:** uint8 (0-100%)  

**Features:**
- Automatic battery percentage calculation (3.3V-4.2V LiPo curve)
- Charging state detection (interrupt-driven)
- OS-level battery display on iOS/Android

### IMU Data Format

18-byte binary packet (little-endian):

| Offset | Type     | Field      | Units          | Description                          |
|--------|----------|------------|----------------|--------------------------------------|
| 0-3    | uint32_t | timestamp  | milliseconds   | System uptime                        |
| 4-5    | int16_t  | ax         | m/s² × 1000    | Accelerometer X-axis                 |
| 6-7    | int16_t  | ay         | m/s² × 1000    | Accelerometer Y-axis                 |
| 8-9    | int16_t  | az         | m/s² × 1000    | Accelerometer Z-axis                 |
| 10-11  | int16_t  | gx         | dps × 1000     | Gyroscope X-axis                     |
| 12-13  | int16_t  | gy         | dps × 1000     | Gyroscope Y-axis                     |
| 14-15  | int16_t  | gz         | dps × 1000     | Gyroscope Z-axis                     |
| 16-17  | int16_t  | vertical   | m/s² × 1000    | Vertical acceleration (rotation-invariant) |

**Example packet:**
```
92 a4 00 00 | 82 ff | fa 00 | 43 26 | 07 00 | f8 ff | f0 ff | 02 00
└─ 42130ms  │ -126  │ +250  │ 9795  │ +7    │ -8    │ -16   │ +2
            └─────────────────────┬─────────────────────────────┘
            Accel (milli-m/s²)    Gyro (milli-dps)    Vertical
```

### Decoding Example (JavaScript)
```javascript
const view = new DataView(data);
const packet = {
  timestamp_ms: view.getUint32(0, true),
  ax: view.getInt16(4, true) / 1000.0,   // Convert to m/s²
  ay: view.getInt16(6, true) / 1000.0,
  az: view.getInt16(8, true) / 1000.0,
  gx: view.getInt16(10, true) / 1000.0,  // Convert to dps
  gy: view.getInt16(12, true) / 1000.0,
  gz: view.getInt16(14, true) / 1000.0,
  vertical: view.getInt16(16, true) / 1000.0
};
```

---

## Prerequisites (Zephyr setup)

This project does NOT include Zephyr or the SDK.

You must install Zephyr separately.

### 1. Create Zephyr workspace

```bash
mkdir -p ~/zephyrproject
cd ~/zephyrproject
python3 -m venv .venv
source .venv/bin/activate
pip install west
```

### 2. Initialize Zephyr

```bash
west init .
west update
west zephyr-export
```

### 3. Install dependencies

Follow official guide:
https://docs.zephyrproject.org/latest/develop/getting_started/index.html

Set environment:

```bash
source ~/zephyrproject/zephyr/zephyr-env.sh
```

or:

```bash
export ZEPHYR_BASE=~/zephyrproject/zephyr
```

---

## Build this firmware

### Option 1: Use the build script (recommended)

From this directory:

```bash
./build.sh          # Incremental build
./build.sh clean    # Clean build
```

The script automatically:
- Activates the Zephyr virtual environment
- Runs `west build` from the correct workspace location
- Shows the output location

### Option 2: Manual build

From the Zephyr workspace directory:

```bash
cd ~/zephyrproject
source .venv/bin/activate
west build -b xiao_ble/nrf52840/sense ~/source/liftsense/liftsense-firmware
```

**Note:** Always run `west build` from the `~/zephyrproject` workspace, not from the application directory. This is required because the application references the Zephyr installation in that workspace.

## Flash (UF2 bootloader)

The build outputs to `~/zephyrproject/build/zephyr/zephyr.uf2`.

To flash:

1. Double-tap the reset button on the XIAO to enter bootloader mode
2. Copy the UF2 file to the mounted drive:

```bash
cp ~/zephyrproject/build/zephyr/zephyr.uf2 /media/$USER/XIAO-SENSE/
```

Or use `west flash` if you have a SWD debugger connected.

---

## Hardware

- **Board:** Seeed Studio XIAO nRF52840 Sense
- **MCU:** nRF52840 (ARM Cortex-M4, 64 MHz)
- **IMU:** LSM6DS3 (I2C)
- **BLE:** Bluetooth 5.0 Low Energy
- **Battery:** 3.7V LiPo with onboard charging (50mA/100mA)

---

## Project Structure

```
src/
├── main.c           - Main application logic
├── imu.c/h          - IMU sensor initialization and handling
├── ble.c/h          - Bluetooth GATT services (IMU + Battery)
├── sensor_math.c/h  - Complementary filter & calculations
├── battery.c/h      - Battery management (voltage, %, charging)
└── imu_packet.h     - BLE data packet definition

dts/
└── bindings/
    └── xiao-ble-battery.yaml - Battery devicetree binding

boards/
└── xiao_ble_nrf52840_sense.overlay - Hardware configuration
```

---

## Testing with iOS/Android

**Recommended Apps:**
- **nRF Connect** (Nordic Semiconductor) - Best for debugging
- **LightBlue** (Punch Through) - User-friendly alternative

**Steps:**
1. Flash firmware to device
2. Open BLE scanner app
3. Look for "LiftSense" device
4. Connect and explore services
5. Find the LiftSense IMU service (`a0f1f001-...`)
6. Find IMU data characteristic (`a0f1f002-...`)
7. Enable notifications
8. View streaming 20-byte packets at 50 Hz
9. **Optional:** Check Battery Service (0x180F) for battery level

---

## Serial Console Output

Connect at 115200 baud to see debug output:

```
LiftSense firmware booting...
IMU initialized at 104 Hz
BLE initialized
BLE advertising started (LiftSense)
Battery: Charger disconnected
Battery: Initialized
Battery: Sampling every 30000 ms
Calibrating gravity vector...
Gravity calibrated: [-0.02, 0.01, 1.00] (mag: 9.81 m/s²)
LiftSense streaming started

CONSOLE: Accel: x=-0.13 y=0.25 z=9.80 m/s² | Gyro: x=0.01 y=-0.01 z=-0.02 dps | Vert: 0.00 m/s²
PACKED:  ts=42130 ax=-126 ay=250 az=9795 gx=7 gy=-8 gz=-16 vert=2
BLE TX:  ts=42130 ax=-126 ay=250 az=9795 gx=7 gy=-8 gz=-16 vert=2
BLE RAW: 92 a4 00 00 | 82 ff | fa 00 | 43 26 | 07 00 | f8 ff | f0 ff | 02 00

Battery: 4150 mV (95%)
```


---

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

---

## Credits

**Battery Management Module:** Adapted from Marcus Alexander Tjomsaas's [XIAO BLE Battery Management Library](https://github.com/Tjoms99/xiao_sense_nrf52840_battery_lib) (Apache-2.0)

# LiftSense Firmware

Zephyr-based firmware for LiftSense IMU + BLE barbell velocity sensor.

## Overview

This project runs on the Seeed XIAO nRF52840 Sense and streams IMU data over BLE for velocity-based training (VBT) applications.

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

From this directory:

```bash
west build -b xiao_ble .
```

## Flash (UF2 bootloader)

```bash
cp build/zephyr/zephyr.uf2 /media/$USER/XIAO-SENSE/
```

or:

```bash
west flash
```

(if SWD is available)

---

## Hardware

- Seeed Studio XIAO nRF52840 Sense
- Built-in IMU (LSM6DS3)

---

## Development notes

- Start with serial logging (printk)
- Then IMU sampling
- Then BLE streaming
- Then VBT analytics

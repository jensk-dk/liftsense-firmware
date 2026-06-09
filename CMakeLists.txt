cmake_minimum_required(VERSION 3.20.0)

# Set DTS_ROOT so Zephyr finds our custom devicetree bindings in dts/bindings/
list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Explicitly specify the overlay file
set(DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/app.overlay)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(liftsense_firmware)

target_sources(app PRIVATE 
    src/main.c
    src/imu.c
    src/ble.c
    src/sensor_math.c
    src/battery.c
)
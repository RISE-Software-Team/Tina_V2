# Sensor Testing with ZMQ Mock Drivers

This directory contains test programs for BME280 and BNO055 sensors that use ZMQ-based mock drivers to receive simulated sensor data from a Python publisher.

## Overview

The test infrastructure uses:
- **Python Publisher** (`sensor_publisher.py`): Publishes simulated sensor data via ZMQ
- **C Mock Drivers**: Connect to the publisher and provide sensor data to the test programs
- **Test Programs**: Read and display sensor values continuously

## Prerequisites

- GCC compiler
- ZMQ library (`libzmq-dev` on Ubuntu/Debian)
- Python 3 with `pyzmq` package

Install dependencies:
```bash
sudo apt-get install libzmq3-dev
pip3 install pyzmq
```

## Building

Build both test programs:
```bash
make
```

Build individual tests:
```bash
make bme280_test
make bno055_test
```

Clean build artifacts:
```bash
make clean
```

## Running

### Step 1: Start the Python Sensor Publisher

In one terminal:
```bash
python3 ../../sensor_publisher.py
```

You should see: `JSON sensor publisher started on tcp://*:5555`

### Step 2: Run Test Programs

In another terminal:

**Test BME280:**
```bash
./bme280_test
```
Output shows temperature (°C), pressure (hPa), and humidity (%)

**Test BNO055:**
```bash
./bno055_test
```
Output shows:
- Accelerometer data (m/s²)
- Gyroscope data (°/s)
- Euler angles: heading, roll, pitch (degrees)

**Run Both Tests:**
```bash
./run_tests.sh
```
This script runs both tests for 5 seconds each.

## Architecture

### BME280 Mock Driver
- Files: `bme280_mock_driver.c`, `bme280_mock_driver.h`
- Subscribes to "BME280" topic
- Parses JSON: `{"temperature":x, "pressure":y, "humidity":z}`
- Adds small random noise to simulate sensor characteristics

### BNO055 Mock Driver
- Files: `bno055_mock_driver.c`, `bno055_mock_driver.h`
- Subscribes to "BNO055" topic
- Parses JSON: `{"accel":[x,y,z], "gyro":[x,y,z], "euler":{...}}`
- Adds small random noise to simulate sensor characteristics

### Communication Flow
```
Python Publisher (port 5555)
    ↓ ZMQ PUB/SUB
    ↓ Topics: "BME280", "BNO055"
    ↓
Mock Drivers (C)
    ↓ API Calls
    ↓
Test Programs
```

## Example Output

### BME280 Test
```
BME280 ZMQ test starting...
BME280 mock: Publisher detected and connected
BME280 initialized successfully
Temp: 24.93 °C | Pressure: 1000.00 hPa | Humidity: 49.54 %
Temp: 25.02 °C | Pressure: 999.92 hPa | Humidity: 50.40 %
```

### BNO055 Test
```
BNO055 ZMQ test starting...
BNO055 mock: Publisher detected and connected
BNO055 initialized successfully
Accel: X=-0.00 m/s² | Y=-0.00 m/s² | Z=-9.80 m/s²
Gyro:  X=-0.13 °/s | Y=-0.37 °/s | Z=0.46 °/s
Euler: Heading=0.00° | Roll=0.06° | Pitch=-0.06°
---
```

## Troubleshooting

**"Failed to connect to ZMQ"**: Make sure `sensor_publisher.py` is running first

**"No publisher detected"**: The mock driver waits 2 seconds for data. If the publisher isn't sending data, the init will fail.

**Compilation errors**: Ensure `libzmq-dev` is installed
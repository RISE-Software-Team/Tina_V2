# Sensor Testing with ZMQ Mock Drivers

This directory contains **mock drivers and test programs** for BME280 and BNO055 sensors that receive **simulated sensor data** from a Python publisher using **ZeroMQ (ZMQ)**.  

The setup allows testing software that relies on sensor data **without actual hardware**, supporting real-time playback and monitoring.

---

## Overview

- **Python Publisher** (`sensor_publisher.py`): Publishes simulated sensor data via ZMQ.
- **C Mock Drivers**: Connect to the publisher and provide sensor data to the test programs.
- **Test Programs**: Read and display sensor values continuously, optionally combined into a single compact output with timestamps.

---

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
make test_sensors_combined
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
./build/bme280_test
```
Output shows temperature (°C), pressure (hPa), and humidity (%)

**Test BNO055:**
```bash
./build/bno055_test
```
Output shows:
- Accelerometer data (m/s²)
- Gyroscope data (°/s)
- Euler angles: heading, roll, pitch (degrees)

**Test Combined:**
```bash
./build/sensors_combined_test
```
Output shows:
- Accelerometer data (m/s²)
- Gyroscope data (°/s)
- Euler angles: heading, roll, pitch (degrees)

## Architecture

### BME280 Mock Driver
- Files: `bme280_mock_driver.c`, `bme280_mock_driver.h`
- Subscribes to "BME280" topic
- Parses JSON: `{"temperature":x, "pressure":y, "humidity":z}`

### BNO055 Mock Driver
- Files: `bno055_mock_driver.c`, `bno055_mock_driver.h`
- Subscribes to "BNO055" topic
- Parses JSON: `{"accel":[x,y,z], "gyro":[x,y,z], "euler":{...}}`

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
```bash
BME280 ZMQ test starting...
BME280 mock: Publisher detected and connected
BME280 initialized successfully
Temp: 24.93 °C | Pressure: 1000.00 hPa | Humidity: 49.54 %
Temp: 25.02 °C | Pressure: 999.92 hPa | Humidity: 50.40 %
---
```

### BNO055 Test
```bash
BNO055 ZMQ test starting...
BNO055 mock: Publisher detected and connected
BNO055 initialized successfully
Accel: X=-0.00 m/s² | Y=-0.00 m/s² | Z=-9.80 m/s²
Gyro:  X=-0.13 °/s | Y=-0.37 °/s | Z=0.46 °/s
Euler: Heading=0.00° | Roll=0.06° | Pitch=-0.06°
---
```

### Combined Sensor Test:
```bash
[1761785902] Temp: 25.32°C | Press: 1002.45 hPa | Hum: 51.23% | Accel: X=0.12 Y=0.05 Z=-9.75 (Mag=9.75) | Gyro: X=1.23 Y=-0.45 Z=0.78 | Euler: H=45.67 R=1.23 P=-0.45
[1761785903] Temp: 24.98°C | Press: 1001.87 hPa | Hum: 49.77% | Accel: X=-0.03 Y=0.01 Z=-9.80 (Mag=9.80) | Gyro: X=0.12 Y=0.05 Z=-0.03 | Euler: H=46.01 R=0.98 P=-0.22
---

## Troubleshooting

**"Failed to connect to ZMQ"**: Make sure `sensor_publisher.py` is running first

**"No publisher detected"**: The mock driver waits 2 seconds for data. If the publisher isn't sending data, the init will fail.

**Compilation errors**: Ensure `libzmq-dev` is installed

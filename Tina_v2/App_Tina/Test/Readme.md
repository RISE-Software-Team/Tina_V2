# Host-side sensor tests

This folder contains host-side test programs for sensor drivers. They are small, self-contained test binaries that use the repository's mock drivers so you can build and run sensor code on your development machine (no hardware required).

Included tests

- `BME280` — host test binary built from `Tina_v2/App_Tina/test/test_bme280.c`
- `BNO055` — host test binary built from `Tina_v2/App_Tina/test/test_bno055.c`

How to run locally

Build and run the BME280 test:

```bash
gcc -std=c99 -I.. -I../Sensors/BME280 -o bme280_test test_bme280.c ../Sensors/BME280/bme280_mock_driver.c -w
```
Sample output:
```bash
➜  Test git:(main) ✗ ./bme280_test
BME280 host test starting...
Temperature: 25.69 C, Pressure: 1000.63 hPa, Humidity: 49.35 %
Temperature: 24.02 C, Pressure: 1000.41 hPa, Humidity: 49.32 %
Temperature: 25.62 C, Pressure: 999.02 hPa, Humidity: 49.54 %
Temperature: 24.90 C, Pressure: 999.02 hPa, Humidity: 50.12 %
Temperature: 25.20 C, Pressure: 1000.08 hPa, Humidity: 50.83 %
```

Build and run the BNO055 test:

```bash
gcc -std=c99 -I.. -I../Sensors/BNO055 -o bno055_test test_bno055.c ../Sensors/BNO055/bno055_mock_driver.c -w
```
Sample output:
```bash
BNO055 host test starting...

Reading 1:
Accel: x=0.057 m/s^2, y=0.036, z=-10.135
Gyro: x=-0.010 deg/s, y=0.004, z=-0.007
Euler: heading=0.000 deg, roll=-0.001 deg, pitch=-0.000 deg

Reading 2:
Accel: x=0.049 m/s^2, y=0.020, z=-9.750
Gyro: x=0.002 deg/s, y=0.001, z=0.008
Euler: heading=-0.000 deg, roll=-0.000 deg, pitch=0.000 deg

Reading 3:
Accel: x=0.052 m/s^2, y=0.023, z=-9.451
Gyro: x=-0.003 deg/s, y=0.003, z=0.004
Euler: heading=0.000 deg, roll=0.000 deg, pitch=0.000 deg

Reading 4:
Accel: x=0.043 m/s^2, y=0.020, z=-9.860
Gyro: x=0.004 deg/s, y=0.003, z=-0.005
Euler: heading=0.000 deg, roll=-0.000 deg, pitch=0.000 deg

Reading 5:
Accel: x=0.057 m/s^2, y=0.025, z=-9.770
Gyro: x=-0.008 deg/s, y=-0.001, z=0.001
Euler: heading=-0.000 deg, roll=-0.001 deg, pitch=0.000 deg
```

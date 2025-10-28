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
```zsh
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
```

Notes

- The tests use small shim headers (`host_includes.h`, `main.h`) in each test directory so they compile with a standard host toolchain.

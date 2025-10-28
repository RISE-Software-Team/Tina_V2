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

Build and run the BNO055 test:

```bash
```

Notes

- The tests use small shim headers (`host_includes.h`, `main.h`) in each test directory so they compile with a standard host toolchain.
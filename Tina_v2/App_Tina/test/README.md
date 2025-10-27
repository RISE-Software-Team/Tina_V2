# Host-side sensor tests

This folder contains host-side test programs for sensor drivers. They are small, self-contained test binaries that use the repository's mock drivers so you can build and run sensor code on your development machine (no hardware required).

Included tests

- `BME280` — host test binary built from `Tina_v2/App_Tina/test/BME280`
- `BNO055` — host test binary built from `Tina_v2/App_Tina/test/BNO055`

How to run locally

Build and run the BME280 test:

```bash
cd Tina_v2/App_Tina/test/BME280
make -C . run
./bme280_test
```

Build and run the BNO055 test:

```bash
cd Tina_v2/App_Tina/test/BNO055
make
./bno055_test
```

You can also build and run both from the repository root:

```bash
make -C Tina_v2/App_Tina/test/BME280 && Tina_v2/App_Tina/test/BME280/bme280_test
make -C Tina_v2/App_Tina/test/BNO055 && Tina_v2/App_Tina/test/BNO055/bno055_test
```

Notes

- The tests use small shim headers (`host_includes.h`, `main.h`) in each test directory so they compile with a standard host toolchain.
- The mock drivers live in `Tina_v2/App_Tina/Sensors/*/*_mock_driver.c` and are registered by the API; the tests simply call the API (init + reads) and print results.

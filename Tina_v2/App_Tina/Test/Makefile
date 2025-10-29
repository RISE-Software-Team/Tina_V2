# Compiler and flags
CC = gcc
CFLAGS = -I../Sensors/BME280 -I../Sensors/BNO055 -Wall
LDFLAGS = -lzmq

# Test source files
SRC = test_bme280.c ../Sensors/BME280/bme280_api.c ../Sensors/BME280/bme280_mock_driver.c \
      test_bno055.c ../Sensors/BNO055/bno055_api.c ../Sensors/BNO055/bno055_mock_driver.c

# Build targets
TARGET_BME280 = bme280_test
TARGET_BNO055 = bno055_test

all: $(TARGET_BME280) $(TARGET_BNO055)

# Build BME280 test
$(TARGET_BME280): test_bme280.c ../Sensors/BME280/bme280_api.c ../Sensors/BME280/bme280_mock_driver.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build BNO055 test
$(TARGET_BNO055): test_bno055.c ../Sensors/BNO055/bno055_api.c ../Sensors/BNO055/bno055_mock_driver.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET_BME280) $(TARGET_BNO055) *.o

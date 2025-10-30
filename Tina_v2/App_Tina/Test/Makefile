# Compiler and flags
CC = gcc
CFLAGS = -I../Sensors/BME280 -I../Sensors/BNO055 -Wall
LDFLAGS = -lzmq -lm

# Build directory
BUILD_DIR = build

# Source files
SRC_BME280 = test_bme280.c ../Sensors/BME280/bme280_api.c ../Sensors/BME280/bme280_mock_driver.c
SRC_BNO055 = test_bno055.c ../Sensors/BNO055/bno055_api.c ../Sensors/BNO055/bno055_mock_driver.c
SRC_SENSOR = test_sensors_combined.c \
             ../Sensors/BME280/bme280_api.c ../Sensors/BME280/bme280_mock_driver.c \
             ../Sensors/BNO055/bno055_api.c ../Sensors/BNO055/bno055_mock_driver.c

# Executables
TARGET_BME280 = $(BUILD_DIR)/bme280_test
TARGET_BNO055 = $(BUILD_DIR)/bno055_test
TARGET_SENSOR = $(BUILD_DIR)/sensors_combined_test

.PHONY: all clean

all: $(BUILD_DIR) $(TARGET_BME280) $(TARGET_BNO055) $(TARGET_SENSOR)

# Create build dir
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build BME280 test
$(TARGET_BME280): $(SRC_BME280) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build BNO055 test
$(TARGET_BNO055): $(SRC_BNO055) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build combined sensor test
$(TARGET_SENSOR): $(SRC_SENSOR) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Clean
clean:
	rm -rf $(BUILD_DIR)

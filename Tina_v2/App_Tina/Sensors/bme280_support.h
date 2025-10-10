#ifndef BME280_SUPPORT_H
#define BME280_SUPPORT_H

#include "main.h"      // For HAL types like I2C_HandleTypeDef, HAL_Delay
#include "bme280.h"    // For the BME280 driver types (struct bme280_t, s8, u8, s32, etc.)

// --- External Dependencies ---

/**
 * @brief I2C Handle structure.
 *
 * This must be defined in one C file (e.g., main.c or i2c.c).
 * The provided code assumes it is hi2c2.
 */
extern I2C_HandleTypeDef hi2c2;

/**
 * @brief Global sensor structure for the BME280 sensor.
 */
extern struct bme280_t bme280;

// --- Public API Functions ---

/**
 * @brief Initializes the BME280 sensor by setting up the I2C interface
 * and calling the core bme280_init function.
 * @return BME280_INIT_VALUE on success, or an error code from bme280_init.
 */
s32 BME280_Init(void);

/**
 * @brief Reads compensated temperature, pressure, and humidity values from the sensor.
 * @param temp Pointer to store the temperature in degrees Celsius (float).
 * @param press Pointer to store the pressure in Pa (float).
 * @param hum Pointer to store the humidity in %RH (float).
 */
int BME280_ReadAll(float *temp, float *press, float *hum);

// --- Private Driver Functions (Declared for Linkage/External Driver Use) ---

/**
 * @brief I2C bus write function conforming to the BME280 driver interface.
 */
s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

/**
 * @brief I2C bus read function conforming to the BME280 driver interface.
 */
s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

/**
 * @brief Delay function conforming to the BME280 driver interface.
 */
void BME280_delay_msek(u32 msek);

#endif /* BME280_SUPPORT_H */

/*
 * bme280_api.h
 *
 *  Created on: Oct 13, 2025
 *      Author: krissal1234
 */

#ifndef BME280_API_H
#define BME280_API_H

#include "Drivers/bme280.h"

// Undefine ERROR macro from bme280.h to prevent conflict with STM32 HAL
#ifdef ERROR
#undef ERROR
#endif

#include "main.h"
#include "types_support.h"

// External I2C handle
extern I2C_HandleTypeDef hi2c2;

// Global BME280 sensor structure
extern struct bme280_t bme280;

// Initialize BME280 sensor
MessageCode_t BME280_Init(void);

// Read all sensor values (temperature, pressure, humidity)
MessageCode_t BME280_ReadAll(f32 *temp, f32 *press, f32 *hum);

// Driver interface functions (used internally by bme280.c driver)
s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void BME280_delay_msek(u32 msek);

#endif /* BME280_API_H */

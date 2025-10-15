/*
	* bme280_api.c
	*
	*  Created on: Oct 13, 2025
	*      Author: krissal1234
	*/

#include "bme280_api.h"

#define BME_I2C_TIMEOUT_MS 100

// Global BME280 sensor structure
struct bme280_t bme280;

// Initialize BME280 sensor
MessageCode_t BME280_Init(void) {
	s32 result;
	
	// Set up I2C communication functions
    bme280.bus_write = BME280_I2C_bus_write;
	bme280.bus_read = BME280_I2C_bus_read;
	bme280.dev_addr = BME280_I2C_ADDRESS1;   // or ADDRESS2 depending on your wiring
    bme280.delay_msec = BME280_delay_msek;
	
	// Initialize the BME280 driver
	result = bme280_init(&bme280);
	if (result != SUCCESS) {
		return ERR_BARO_INIT_FAIL;
	}
	
	// Set sensor to normal mode with standard settings
	result = bme280_set_power_mode(BME280_NORMAL_MODE);
	if (result != SUCCESS) {
		return ERR_BARO_INIT_FAIL;
	}
	
	// Configure oversampling: temp x1, pressure x16, humidity x1
	result = bme280_set_oversamp_temperature(BME280_OVERSAMP_1X);
	if (result != SUCCESS) {
		return ERR_BARO_INIT_FAIL;
	}
	
	result = bme280_set_oversamp_pressure(BME280_OVERSAMP_16X);
	if (result != SUCCESS) {
		return ERR_BARO_INIT_FAIL;
	}
	
	result = bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);
	if (result != SUCCESS) {
		return ERR_BARO_INIT_FAIL;
	}
	
	// Set standby time to 1ms
	result = bme280_set_standby_durn(BME280_STANDBY_TIME_1_MS);
	if (result != SUCCESS) {
		return ERR_BARO_INIT_FAIL;
	}
	
	// Set IIR filter coefficient to 16 for noise reduction
	result = bme280_set_filter(BME280_FILTER_COEFF_8);
	if (result != SUCCESS) {
		return ERR_BARO_INIT_FAIL;
	}
	
	return INFO_COMPONENT_SANITY_CHECK_PASS;
}

// Read all sensor values
MessageCode_t BME280_ReadAll(f32 *temp, f32 *press, f32 *hum) {
	s32 result;
	s32 temp_raw, press_raw, hum_raw;
	
	// Null pointer check
	if (!temp || !press || !hum) {
		return ERR_BARO_FAIL;
	}
	
	// Read uncompensated data
	result = bme280_read_uncomp_pressure_temperature_humidity(
		&press_raw, &temp_raw, &hum_raw);
	
	if (result != SUCCESS) {
		return ERR_BARO_FAIL;
	}
	
	// Convert to compensated values
	// Temperature in degrees Celsius
	*temp = (f32)bme280_compensate_temperature_double(temp_raw);
	
	// Pressure in Pa (Pascals)
	*press = (f32)bme280_compensate_pressure_double(press_raw);
	
	// Humidity in %RH
	*hum = (f32)bme280_compensate_humidity_double(hum_raw);
	
	return INFO_COMPONENT_SANITY_CHECK_PASS;
}

// I2C write function for BME280 driver
s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
	HAL_StatusTypeDef status;
	
	status = HAL_I2C_Mem_Write(&hi2c2, (dev_addr << 1), reg_addr,
								I2C_MEMADD_SIZE_8BIT, reg_data, cnt, BME_I2C_TIMEOUT_MS);
	
	return (status == HAL_OK) ? SUCCESS : ERROR;
}

// I2C read function for BME280 driver
s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
	HAL_StatusTypeDef status;
	
	status = HAL_I2C_Mem_Read(&hi2c2, (dev_addr << 1), reg_addr,
								I2C_MEMADD_SIZE_8BIT, reg_data, cnt, BME_I2C_TIMEOUT_MS);
	
	return (status == HAL_OK) ? SUCCESS : ERROR;
}

// Delay function for BME280 driver
void BME280_delay_msek(u32 msek) {
	HAL_Delay(msek);
}

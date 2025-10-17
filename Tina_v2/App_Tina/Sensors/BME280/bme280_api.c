/*
 * bme280_api.c
 *
 *  Created on: Oct 13, 2025
 *      Author: krissal1234
 */

#include "main.h"
#include "bme280.h"
#include "bme280_api.h"

#define BME280_API
#ifdef BME280_API

static struct bme280_t bme280_sensor;
static bool is_initialized;
static BME280_HAL_t current_hal;

extern I2C_HandleTypeDef hi2c2;

static s8 default_i2c_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Write(&hi2c2, dev_addr << 1, reg_addr,
				   I2C_MEMADD_SIZE_8BIT, reg_data, cnt, 100);
	return (status == HAL_OK) ? 0 : -1;
}

static s8 default_i2c_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Read(&hi2c2, dev_addr << 1, reg_addr,
				  I2C_MEMADD_SIZE_8BIT, reg_data, cnt, 100);
	return (status == HAL_OK) ? 0 : -1;
}

static void default_delay_ms(u32 msec)
{
	HAL_Delay(msec);
}

static BME280_Status_t configure_sensor_settings(void)
{
	s32 result;

	result = bme280_set_oversamp_temperature(BME280_OVERSAMP_1X);
	if (result != SUCCESS)
		return BME280_ERROR_CONFIG;

	result = bme280_set_oversamp_pressure(BME280_OVERSAMP_16X);
	if (result != SUCCESS)
		return BME280_ERROR_CONFIG;

	result = bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);
	if (result != SUCCESS)
		return BME280_ERROR_CONFIG;

	result = bme280_set_standby_durn(BME280_STANDBY_TIME_1_MS);
	if (result != SUCCESS)
		return BME280_ERROR_CONFIG;

	result = bme280_set_filter(BME280_FILTER_COEFF_8);
	if (result != SUCCESS)
		return BME280_ERROR_CONFIG;

	return BME280_OK;
}

BME280_Status_t BME280_Init(const BME280_HAL_t *hal)
{
	s32 result;
	BME280_Status_t status;

	if (hal)
		current_hal = *hal;
	else {
		current_hal.i2c_write = default_i2c_write;
		current_hal.i2c_read = default_i2c_read;
		current_hal.delay_ms = default_delay_ms;
	}

	bme280_sensor.bus_write = current_hal.i2c_write;
	bme280_sensor.bus_read = current_hal.i2c_read;
	bme280_sensor.delay_msec = current_hal.delay_ms;
	bme280_sensor.dev_addr = BME280_I2C_ADDRESS1;

	result = bme280_init(&bme280_sensor);
	if (result != SUCCESS) {
		is_initialized = false;
		return BME280_ERROR_INIT;
	}

	status = configure_sensor_settings();
	if (status != BME280_OK) {
		is_initialized = false;
		return status;
	}

	result = bme280_set_power_mode(BME280_NORMAL_MODE);
	if (result != SUCCESS) {
		is_initialized = false;
		return BME280_ERROR_INIT;
	}

	is_initialized = true;
	return BME280_OK;
}

BME280_Status_t BME280_DeInit(void)
{
	s32 result;

	if (!is_initialized)
		return BME280_ERROR_NOT_INITIALIZED;

	result = bme280_set_power_mode(BME280_SLEEP_MODE);
	if (result != SUCCESS)
		return BME280_ERROR_COMM;

	is_initialized = false;
	return BME280_OK;
}

BME280_Status_t BME280_ReadPressure(float *pressure_Pa)
{
	s32 uncomp_pressure;
	u32 comp_pressure;
	s32 result;

	if (!is_initialized)
		return BME280_ERROR_NOT_INITIALIZED;

	if (!pressure_Pa)
		return BME280_ERROR_INVALID_PARAM;

	result = bme280_read_uncomp_pressure(&uncomp_pressure);
	if (result != SUCCESS)
		return BME280_ERROR_COMM;

	comp_pressure = bme280_compensate_pressure_int32(uncomp_pressure);
	*pressure_Pa = (float)comp_pressure / 256.0f;

	return BME280_OK;
}

BME280_Status_t BME280_ReadTemperature(float *temperature_C)
{
	s32 uncomp_temperature;
	s32 comp_temperature;
	s32 result;

	if (!is_initialized)
		return BME280_ERROR_NOT_INITIALIZED;

	if (!temperature_C)
		return BME280_ERROR_INVALID_PARAM;

	result = bme280_read_uncomp_temperature(&uncomp_temperature);
	if (result != SUCCESS)
		return BME280_ERROR_COMM;

	comp_temperature = bme280_compensate_temperature_int32(uncomp_temperature);
	*temperature_C = (float)comp_temperature / 100.0f;

	return BME280_OK;
}

BME280_Status_t BME280_ReadHumidity(float *humidity_RH)
{
	s32 uncomp_humidity;
	u32 comp_humidity;
	s32 result;

	if (!is_initialized)
		return BME280_ERROR_NOT_INITIALIZED;

	if (!humidity_RH)
		return BME280_ERROR_INVALID_PARAM;

	result = bme280_read_uncomp_humidity(&uncomp_humidity);
	if (result != SUCCESS)
		return BME280_ERROR_COMM;

	comp_humidity = bme280_compensate_humidity_int32(uncomp_humidity);
	*humidity_RH = (float)comp_humidity / 1024.0f;

	return BME280_OK;
}

BME280_Status_t BME280_ReadAll(BME280_Data_t *data)
{
	s32 uncomp_temp, uncomp_press, uncomp_hum;
	s32 comp_temp;
	u32 comp_press, comp_hum;
	s32 result;

	if (!is_initialized)
		return BME280_ERROR_NOT_INITIALIZED;

	if (!data)
		return BME280_ERROR_INVALID_PARAM;

	data->valid = false;

	result = bme280_read_uncomp_pressure_temperature_humidity(
		&uncomp_press, &uncomp_temp, &uncomp_hum);
	if (result != SUCCESS)
		return BME280_ERROR_COMM;

	comp_temp = bme280_compensate_temperature_int32(uncomp_temp);
	comp_press = bme280_compensate_pressure_int32(uncomp_press);
	comp_hum = bme280_compensate_humidity_int32(uncomp_hum);

	data->temperature_C = (float)comp_temp / 100.0f;
	data->pressure_Pa = (float)comp_press / 256.0f;
	data->humidity_RH = (float)comp_hum / 1024.0f;
	data->valid = true;

	return BME280_OK;
}

bool BME280_IsInitialized(void)
{
	return is_initialized;
}

BME280_Status_t BME280_SoftReset(void)
{
	s32 result;

	if (!is_initialized)
		return BME280_ERROR_NOT_INITIALIZED;

	result = bme280_set_soft_rst();
	if (result != SUCCESS)
		return BME280_ERROR_COMM;

	current_hal.delay_ms(10);
	return BME280_OK;
}

#endif /* BME280_API */
/*
 * bme280_api.c
 *
 *  Created on: Oct 22, 2025
 *      Author: pranavsuri4303
 */

#include "bme280_api.h"

// Default hardware driver (defined in bme280_hw_driver.c)
extern const BME280_Driver_t bme280_default_driver;

// Pointer to the currently registered driver (mock or hardware)
static const BME280_Driver_t *driver = &bme280_default_driver;

/* -------------------------------------------------------------------------- */
/* Driver registration and retrieval                                          */
/* -------------------------------------------------------------------------- */
void BME280_RegisterDriver(const BME280_Driver_t *new_driver)
{
	if (new_driver)
		driver = new_driver;
}

/* -------------------------------------------------------------------------- */
/* Public wrapper API (calls into registered driver)                          */
/* -------------------------------------------------------------------------- */
int32_t BME280_Init(void)
{
    return driver->init ? driver->init() : -1;
}

int BME280_ReadTemperature(float *temp)
{
    return driver->read_temperature ? driver->read_temperature(temp) : -1;
}

int BME280_ReadPressure(float *press)
{
    return driver->read_pressure ? driver->read_pressure(press) : -1;
}

int BME280_ReadHumidity(float *hum)
{
    return driver->read_humidity ? driver->read_humidity(hum) : -1;
}

int BME280_ReadAll(float *temp, float *press, float *hum)
{
    return driver->read_all ? driver->read_all(temp, press, hum) : -1;
}

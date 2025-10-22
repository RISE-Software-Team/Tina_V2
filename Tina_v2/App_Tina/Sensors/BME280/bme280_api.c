/*
 * bme280_api.c
 *
 *  Created on: Oct 22, 2025
 *      Author: pranavsuri4303
 */

#include "bme280_api.h"

// Pointer to the currently registered driver (mock or hardware)
static const BME280_Driver_t *registered_driver = NULL;

// Default hardware driver (defined in bme280_hw_driver.c)
extern const BME280_Driver_t default_driver;

/* -------------------------------------------------------------------------- */
/* Driver registration and retrieval                                          */
/* -------------------------------------------------------------------------- */
void BME280_RegisterDriver(const BME280_Driver_t *driver)
{
    registered_driver = (driver != NULL) ? driver : &default_driver;
}

const BME280_Driver_t *BME280_GetRegisteredDriver(void)
{
    return registered_driver ? registered_driver : &default_driver;
}

/* -------------------------------------------------------------------------- */
/* Public wrapper API (calls into registered driver)                          */
/* -------------------------------------------------------------------------- */
s32 BME280_Init(void)
{
    const BME280_Driver_t *drv = BME280_GetRegisteredDriver();
    return drv->init ? drv->init() : -1;
}

int BME280_ReadTemperature(float *temp)
{
    const BME280_Driver_t *drv = BME280_GetRegisteredDriver();
    return drv->read_temperature ? drv->read_temperature(temp) : -1;
}

int BME280_ReadPressure(float *press)
{
    const BME280_Driver_t *drv = BME280_GetRegisteredDriver();
    return drv->read_pressure ? drv->read_pressure(press) : -1;
}

int BME280_ReadHumidity(float *hum)
{
    const BME280_Driver_t *drv = BME280_GetRegisteredDriver();
    return drv->read_humidity ? drv->read_humidity(hum) : -1;
}

int BME280_ReadAll(float *temp, float *press, float *hum)
{
    const BME280_Driver_t *drv = BME280_GetRegisteredDriver();
    return drv->read_all ? drv->read_all(temp, press, hum) : -1;
}
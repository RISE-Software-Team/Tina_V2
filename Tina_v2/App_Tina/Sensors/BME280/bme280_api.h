/*
 * bme280_api.h
 *
 *  Created on: Oct 13, 2025
 *      Author: krissal1234
 */

#ifndef BME280_API_H
#define BME280_API_H

#include "main.h"
#include "bme280.h"
#include <stdbool.h>

typedef enum {
    BME280_OK = 0,
    BME280_ERROR_INIT = -1,
    BME280_ERROR_CONFIG = -2,
    BME280_ERROR_COMM = -3,
    BME280_ERROR_INVALID_PARAM = -4,
    BME280_ERROR_NOT_INITIALIZED = -5,
    BME280_ERROR_TIMEOUT = -6
} BME280_Status_t;

typedef struct {
    float temperature_C;
    float pressure_Pa;
    float humidity_RH;
    bool valid;
} BME280_Data_t;

typedef struct {
    s8 (*i2c_write)(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
    s8 (*i2c_read)(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
    void (*delay_ms)(u32 msek);
} BME280_HAL_t;

BME280_Status_t BME280_Init(const BME280_HAL_t *hal);
BME280_Status_t BME280_DeInit(void);
BME280_Status_t BME280_ReadPressure(float *pressure_Pa);
BME280_Status_t BME280_ReadTemperature(float *temperature_C);
BME280_Status_t BME280_ReadHumidity(float *humidity_RH);
BME280_Status_t BME280_ReadAll(BME280_Data_t *data);
bool BME280_IsInitialized(void);
BME280_Status_t BME280_SoftReset(void);

#endif /* BME280_API_H */

#ifndef BME280_API_H
#define BME280_API_H

#include "main.h"
#include "bme280.h"

// -----------------------------------------------------------------------------
// External dependencies
// -----------------------------------------------------------------------------
extern I2C_HandleTypeDef hi2c2;
extern struct bme280_t bme280;

// -----------------------------------------------------------------------------
// Driver interface for dependency injection / mocking
// -----------------------------------------------------------------------------

typedef struct {
    s32 (*init)(void);
    int (*read_temperature)(float *temp);
    int (*read_pressure)(float *press);
    int (*read_humidity)(float *hum);
    int (*read_all)(float *temp, float *press, float *hum);
} BME280_Driver_t;

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------
void BME280_RegisterDriver(const BME280_Driver_t *driver);
const BME280_Driver_t *BME280_GetRegisteredDriver(void);

s32 BME280_Init(void);
int BME280_ReadTemperature(float *temp);
int BME280_ReadPressure(float *press);
int BME280_ReadHumidity(float *hum);
int BME280_ReadAll(float *temp, float *press, float *hum);

#endif /* BME280_API_H */

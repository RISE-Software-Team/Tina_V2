#ifndef BME280_API_H
#define BME280_API_H

#include <stdint.h>
#include <stddef.h>

// -----------------------------------------------------------------------------
// Data types for sensor readings
// -----------------------------------------------------------------------------
typedef struct {
    float temperature;  // °C
    float pressure;     // hPa
    float humidity;     // %
} BME280_Data_t;

// -----------------------------------------------------------------------------
// Driver interface for dependency injection / mocking
// -----------------------------------------------------------------------------
typedef struct {
    int32_t (*init)(void);
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

int32_t BME280_Init(void);
int BME280_ReadTemperature(float *temp);
int BME280_ReadPressure(float *press);
int BME280_ReadHumidity(float *hum);
int BME280_ReadAll(float *temp, float *press, float *hum);

#endif /* BME280_API_H */

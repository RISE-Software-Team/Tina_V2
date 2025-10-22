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

// -----------------------------------------------------------------------------
// Driver interface for dependency injection / mocking
// -----------------------------------------------------------------------------

typedef struct {
    /**
     * Initialize the sensor / driver.
     * Return 0 on success or negative error code.
     */
    s32 (*init)(void);
    int (*read_temperature)(float *temp);
    int (*read_pressure)(float *press);
    int (*read_humidity)(float *hum);
    int (*read_all)(float *temp, float *press, float *hum);
} BME280_Driver_t;

void BME280_RegisterDriver(const BME280_Driver_t *driver);
const BME280_Driver_t *BME280_GetRegisteredDriver(void);

s32 BME280_Init(void);

int BME280_ReadTemperature(float *temp);
int BME280_ReadPressure(float *press);
int BME280_ReadHumidity(float *hum);
int BME280_ReadAll(float *temp, float *press, float *hum);

#endif /* BME280_SUPPORT_H */

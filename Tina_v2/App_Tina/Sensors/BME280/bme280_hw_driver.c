#include "bme280_api.h"
#include "bme280.h"
#include "main.h"

/* -------------------------------------------------------------------------- */
/* External dependencies                                                      */
/* -------------------------------------------------------------------------- */
extern I2C_HandleTypeDef hi2c2;
static struct bme280_t bme280;

/* -------------------------------------------------------------------------- */
/* Low-level I2C HAL wrappers                                                 */
/* -------------------------------------------------------------------------- */
static s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
static s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
static void BME280_delay_msek(u32 msek);

/* -------------------------------------------------------------------------- */
/* Hardware-level operations                                                  */
/* -------------------------------------------------------------------------- */
static int32_t hw_init(void);
static int hw_read_all(float *temp, float *press, float *hum);
static int hw_read_temperature(float *temp);
static int hw_read_pressure(float *press);
static int hw_read_humidity(float *hum);

/* -------------------------------------------------------------------------- */
/* Default hardware driver instance                                           */
/* -------------------------------------------------------------------------- */
const BME280_Driver_t bme280_default_driver = {
    .init = hw_init,
    .read_temperature = hw_read_temperature,
    .read_pressure = hw_read_pressure,
    .read_humidity = hw_read_humidity,
    .read_all = hw_read_all,
};

/* -------------------------------------------------------------------------- */
/* BME280 I2C setup routine                                                   */
/* -------------------------------------------------------------------------- */
static s8 BME280_I2C_routine(void)
{
    bme280.bus_write  = BME280_I2C_bus_write;
    bme280.bus_read   = BME280_I2C_bus_read;
    bme280.dev_addr   = BME280_I2C_ADDRESS1;
    bme280.delay_msec = BME280_delay_msek;
    return BME280_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Initialize BME280 hardware                                                 */
/* -------------------------------------------------------------------------- */
static int32_t hw_init(void)
{
    BME280_I2C_routine();
    s32 rslt = bme280_init(&bme280);
    if (rslt != BME280_SUCCESS) return rslt;

    bme280_set_oversamp_temperature(BME280_OVERSAMP_1X);
    bme280_set_oversamp_pressure(BME280_OVERSAMP_16X);
    bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);
    bme280_set_power_mode(BME280_NORMAL_MODE);
    return rslt;
}

static int hw_read_all(float *temp, float *press, float *hum)
{
    s32 uncomp_temp = 0, uncomp_pres = 0, uncomp_hum = 0;
    if (bme280_read_uncomp_pressure_temperature_humidity(&uncomp_pres, &uncomp_temp, &uncomp_hum) != BME280_SUCCESS)
        return -1;

    if (temp)  *temp  = (float)bme280_compensate_temperature_int32(uncomp_temp) / 100.0f; // C
    if (press) *press = (float)bme280_compensate_pressure_int32(uncomp_pres);             // Pa
    if (hum)   *hum   = (float)bme280_compensate_humidity_int32(uncomp_hum) / 1024.0f;
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Individual sensor read wrappers                                            */
/* -------------------------------------------------------------------------- */
static int hw_read_temperature(float *temp) { return hw_read_all(temp, NULL, NULL); }
static int hw_read_pressure(float *press)   { return hw_read_all(NULL, press, NULL); }
static int hw_read_humidity(float *hum)     { return hw_read_all(NULL, NULL, hum); }

/* -------------------------------------------------------------------------- */
/* HAL I2C low-level read/write/delay implementations                         */
/* -------------------------------------------------------------------------- */
static s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    uint16_t hal_addr = (uint16_t)(dev_addr << 1);
    HAL_StatusTypeDef rc = HAL_I2C_Mem_Write(
        &hi2c2,
        hal_addr,
        (uint16_t)reg_addr,
        I2C_MEMADD_SIZE_8BIT,
        reg_data,
        (uint16_t)cnt,
        100
    );
    return (rc == HAL_OK) ? 0 : -1;
}

static s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    uint16_t hal_addr = (uint16_t)(dev_addr << 1);
    HAL_StatusTypeDef rc = HAL_I2C_Mem_Read(
        &hi2c2,
        hal_addr,
        (uint16_t)reg_addr,
        I2C_MEMADD_SIZE_8BIT,
        reg_data,
        (uint16_t)cnt,
        100
    );
    return (rc == HAL_OK) ? 0 : -1;
}

static void BME280_delay_msek(u32 msek)
{
    HAL_Delay(msek);
}

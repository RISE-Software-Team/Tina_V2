#include "bme280_support.h"
#include "logger.h"
#include <stdio.h>

#define BME280_API

#ifdef BME280_API

/* -------------------------------------------------------------------------- */
/* GLOBALS                                                                    */
/* -------------------------------------------------------------------------- */

extern I2C_HandleTypeDef hi2c2;
struct bme280_t bme280;

/* -------------------------------------------------------------------------- */
/* INTERNAL DRIVER STRUCT AND REGISTRATION                                    */
/* -------------------------------------------------------------------------- */

/* Forward declarations for the default hardware driver */
static s32 hw_init(void);
static int hw_read_temperature(float *temp);
static int hw_read_pressure(float *press);
static int hw_read_humidity(float *hum);
static int hw_read_all(float *temp, float *press, float *hum);
static const BME280_Driver_t *registered_driver = NULL;

/* Default hardware driver instance */
static const BME280_Driver_t default_driver = {
    .init = hw_init,
    .read_temperature = hw_read_temperature,
    .read_pressure = hw_read_pressure,
    .read_humidity = hw_read_humidity,
    .read_all = hw_read_all,
};

/* -------------------------------------------------------------------------- */
/* DRIVER REGISTRATION / RETRIEVAL                                            */
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
/* PUBLIC WRAPPERS (CALLABLE FROM FSM OR APP CODE)                            */
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

/* -------------------------------------------------------------------------- */
/* LOW-LEVEL HARDWARE IMPLEMENTATION                                          */
/* -------------------------------------------------------------------------- */

static s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    if (HAL_I2C_Mem_Write(&hi2c2, dev_addr << 1, reg_addr,
                          I2C_MEMADD_SIZE_8BIT, reg_data, cnt, 100) != HAL_OK)
        return -1;
    return 0;
}

static s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    if (HAL_I2C_Mem_Read(&hi2c2, dev_addr << 1, reg_addr,
                         I2C_MEMADD_SIZE_8BIT, reg_data, cnt, 100) != HAL_OK)
        return -1;
    return 0;
}

static void BME280_delay_msek(u32 msek)
{
    HAL_Delay(msek);
}

/* -------------------------------------------------------------------------- */
/* DEFAULT HARDWARE DRIVER IMPLEMENTATION                                     */
/* -------------------------------------------------------------------------- */

static s8 I2C_routine(void)
{
    bme280.bus_write = BME280_I2C_bus_write;
    bme280.bus_read  = BME280_I2C_bus_read;
    bme280.dev_addr  = BME280_I2C_ADDRESS1;   /* or ADDRESS2 depending on wiring */
    bme280.delay_msec = BME280_delay_msek;
    return 0;
}

static s32 hw_init(void)
{
    I2C_routine();
    s32 rslt = bme280_init(&bme280);
    if (rslt != 0) return rslt;

    bme280_set_oversamp_temperature(BME280_OVERSAMP_1X);
    bme280_set_oversamp_pressure(BME280_OVERSAMP_1X);
    bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);
    bme280_set_power_mode(BME280_NORMAL_MODE);

    return rslt;
}

static int hw_read_all(float *temp, float *press, float *hum)
{
    s32 uncomp_temp = 0, uncomp_pres = 0, uncomp_hum = 0;
    if (bme280_read_uncomp_pressure_temperature_humidity(&uncomp_pres, &uncomp_temp, &uncomp_hum) != 0)
        return -1;

    if (temp)  *temp  = (float)bme280_compensate_temperature_int32(uncomp_temp) / 100.0f;
    if (press) *press = (float)bme280_compensate_pressure_int32(uncomp_pres) / 100.0f;
    if (hum)   *hum   = (float)bme280_compensate_humidity_int32(uncomp_hum) / 1024.0f;
    return 0;
}

static int hw_read_temperature(float *temp)
{
    return hw_read_all(temp, NULL, NULL);
}

static int hw_read_pressure(float *press)
{
    return hw_read_all(NULL, press, NULL);
}

static int hw_read_humidity(float *hum)
{
    return hw_read_all(NULL, NULL, hum);
}

#endif


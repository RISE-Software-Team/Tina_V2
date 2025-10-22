#include "main.h"
#include "bme280.h"
#include "logger.h"
#include <stdio.h>
#define BME280_API

#ifdef BME280_API

extern I2C_HandleTypeDef hi2c2; // assuming you use hi2c1

/* Global sensor struct */
struct bme280_t bme280;

/* I2C function prototypes */
s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

void BME280_delay_msek(u32 msek);

s8 I2C_routine(void)
{
    bme280.bus_write = BME280_I2C_bus_write;
    bme280.bus_read  = BME280_I2C_bus_read;
    bme280.dev_addr  = BME280_I2C_ADDRESS1;   // or ADDRESS2 depending on your wiring
    bme280.delay_msec = BME280_delay_msek;
    return BME280_INIT_VALUE;
}

s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    if (HAL_I2C_Mem_Write(&hi2c2, dev_addr << 1, reg_addr,
                          I2C_MEMADD_SIZE_8BIT, reg_data, cnt, 100) != HAL_OK)
        return -1;
    return 0;
}

s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    if (HAL_I2C_Mem_Read(&hi2c2, dev_addr << 1, reg_addr,
                         I2C_MEMADD_SIZE_8BIT, reg_data, cnt, 100) != HAL_OK)
        return -1;
    return 0;
}

void BME280_delay_msek(u32 msek)
{
    HAL_Delay(msek);
}

/* Simple initialization */
s32 BME280_Init(void)
{
//    I2C_routine();
//    return bme280_init(&bme280);
    I2C_routine();
    s32 rslt = bme280_init(&bme280);

    bme280_set_oversamp_temperature(BME280_OVERSAMP_1X);
    bme280_set_oversamp_pressure(BME280_OVERSAMP_1X);
    bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);

    bme280_set_power_mode(BME280_NORMAL_MODE);

    return rslt;
}

/* Read temperature, pressure, humidity once */
//void BME280_ReadAll(float *temp, float *press, float *hum)
//{
//    s32 uncomp_temp, uncomp_pres, uncomp_hum;
//    bme280_read_uncomp_pressure_temperature_humidity(&uncomp_pres, &uncomp_temp, &uncomp_hum);
//
//    *temp = (float)bme280_compensate_temperature_int32(uncomp_temp) / 100.0f;
//    *press = (float)bme280_compensate_pressure_int32(uncomp_pres) / 100.0f;
//    *hum = (float)bme280_compensate_humidity_int32(uncomp_hum) / 1024.0f;
//}
#include "bme280_support.h"

/* Read temperature, pressure, humidity once, returns 0 on success, nonzero on failure */
int BME280_ReadAll(float *temp, float *press, float *hum)
{
    s32 uncomp_temp = 0, uncomp_pres = 0, uncomp_hum = 0;

    BME280_RETURN_FUNCTION_TYPE status =
        bme280_read_uncomp_pressure_temperature_humidity(&uncomp_pres, &uncomp_temp, &uncomp_hum);
		char log_msg[64];

//    	snprintf(log_msg, sizeof(log_msg), "uncomp_pres: %ld ", uncomp_pres);
//    	tlog(ERR_BARO_CALIB_FAIL, log_msg);
//
//    	snprintf(log_msg, sizeof(log_msg), "status: %d ", status);
//    	tlog(ERR_BARO_CALIB_FAIL, log_msg);

    if (status == ERROR) {
        return -1;
    }

    *temp  = (float)bme280_compensate_temperature_int32(uncomp_temp) / 100.0f;
    *press = (float)bme280_compensate_pressure_int32(uncomp_pres) / 100.0f;
    *hum   = (float)bme280_compensate_humidity_int32(uncomp_hum) / 1024.0f;

    return 0;
}

#endif


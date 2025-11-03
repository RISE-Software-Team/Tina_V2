/*
 * bno055_hw_driver.c
 *
 *  Created on: Oct 22, 2025
 *      Author: pranavsuri4303
 */

#include "bno055_api.h"
#include "bno055.h"
#include "main.h"

/* -------------------------------------------------------------------------- */
/* External dependencies                                                      */
/* -------------------------------------------------------------------------- */
extern I2C_HandleTypeDef hi2c2;
static struct bno055_t bno055;

/* -------------------------------------------------------------------------- */
/* Low-level I2C HAL wrappers                                                 */
/* -------------------------------------------------------------------------- */
static s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
static s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
static void BNO055_delay_msek(u32 msek);

/* -------------------------------------------------------------------------- */
/* Hardware-level operations                                                  */
/* -------------------------------------------------------------------------- */
static s8 hw_init(void);
static int hw_read_accel(BNO055_AccelData_t *accel);
static int hw_read_gyro(BNO055_GyroData_t *gyro);

/* -------------------------------------------------------------------------- */
/* Default hardware driver instance                                           */
/* -------------------------------------------------------------------------- */
const BNO055_Driver_t bno055_default_driver = {
    .init = hw_init,
    .read_accel = hw_read_accel,
    .read_gyro  = hw_read_gyro,
};

/* -------------------------------------------------------------------------- */
/* BNO055 I2C setup routine                                                   */
/* -------------------------------------------------------------------------- */
static s8 BNO055_I2C_routine(void)
{
    bno055.bus_write  = BNO055_I2C_bus_write;
    bno055.bus_read   = BNO055_I2C_bus_read;
    bno055.dev_addr   = BNO055_I2C_ADDR1;
    bno055.delay_msec = BNO055_delay_msek;
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Initialize BNO055 hardware                                                 */
/* -------------------------------------------------------------------------- */
static s8 hw_init(void)
{
    BNO055_I2C_routine();

    s8 rslt;

    rslt = bno055_init(&bno055);
    if (rslt != BNO055_SUCCESS)
    	return rslt;


    rslt = bno055_set_accel_range(BNO055_ACCEL_RANGE_8G);
    if (rslt != BNO055_SUCCESS)
    	return rslt;

    rslt = bno055_set_gyro_range(BNO055_GYRO_RANGE_2000DPS);
    if (rslt != BNO055_SUCCESS)
    	return rslt;

    return bno055_set_operation_mode(BNO055_OPERATION_MODE_ACCGYRO);
}

/* -------------------------------------------------------------------------- */
/* Sensor Read Implementations                                                */
/* -------------------------------------------------------------------------- */
static int hw_read_accel(BNO055_AccelData_t *accel)
{
    struct bno055_accel_float_t data;
    if (bno055_convert_float_accel_xyz_msq(&data) != BNO055_SUCCESS)
        return -1;

    accel->x = data.x;
    accel->y = data.y;
    accel->z = -data.z;
    return 0;
}

static int hw_read_gyro(BNO055_GyroData_t *gyro)
{
    struct bno055_gyro_float_t data;
    if (bno055_convert_float_gyro_xyz_dps(&data) != BNO055_SUCCESS)
        return -1;

    gyro->x = data.x;
    gyro->y = data.y;
    gyro->z = data.z;
    return 0;
}

/* -------------------------------------------------------------------------- */
/* HAL I2C low-level read/write/delay implementations                         */
/* -------------------------------------------------------------------------- */

static s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *data, u8 len)
{
    uint16_t hal_addr = (uint16_t)(dev_addr << 1);
    HAL_StatusTypeDef rc = HAL_I2C_Mem_Write(
        &hi2c2,
        hal_addr,
        (uint16_t)reg_addr,
        I2C_MEMADD_SIZE_8BIT,
        data,
        (uint16_t)len,
        100
    );
    return (rc == HAL_OK) ? BNO055_SUCCESS : BNO055_ERROR;
}

static s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *data, u8 len)
{
    uint16_t hal_addr = (uint16_t)(dev_addr << 1);
    HAL_StatusTypeDef rc = HAL_I2C_Mem_Read(
        &hi2c2,
        hal_addr,
        (uint16_t)reg_addr,
        I2C_MEMADD_SIZE_8BIT,
        data,
        (uint16_t)len,
        100
    );
    return (rc == HAL_OK) ? BNO055_SUCCESS : BNO055_ERROR;
}

static void BNO055_delay_msek(BNO055_MDELAY_DATA_TYPE ms)
{
    HAL_Delay((uint32_t)ms);
}

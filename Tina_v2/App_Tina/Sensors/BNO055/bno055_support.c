#include "main.h"
#include "bno055_support.h"

#define BNO_I2C_TIMEOUT_MS  500

extern I2C_HandleTypeDef hi2c2;

/* Global sensor state */
struct bno055_t bno055;

static s8 bno055_i2c_read(u8 dev_addr, u8 reg_addr, u8 *data, u8 len)
{
    uint16_t hal_addr = (uint16_t)(dev_addr << 1);
    HAL_StatusTypeDef rc = HAL_I2C_Mem_Read(&hi2c2,
                                           hal_addr,
                                           (uint16_t)reg_addr,
                                           I2C_MEMADD_SIZE_8BIT,
                                           data,
                                           (uint16_t)len,
                                           BNO_I2C_TIMEOUT_MS);
    return (rc == HAL_OK) ? BNO055_SUCCESS : BNO055_ERROR;
}

static s8 bno055_i2c_write(u8 dev_addr, u8 reg_addr, u8 *data, u8 len)
{
    uint16_t hal_addr = (uint16_t)(dev_addr << 1);
    HAL_StatusTypeDef rc = HAL_I2C_Mem_Write(&hi2c2,
                                            hal_addr,
                                            (uint16_t)reg_addr,
                                            I2C_MEMADD_SIZE_8BIT,
                                            data,
                                            (uint16_t)len,
                                            BNO_I2C_TIMEOUT_MS);
    return (rc == HAL_OK) ? BNO055_SUCCESS : BNO055_ERROR;
}

static void bno055_delay_ms(BNO055_MDELAY_DATA_TYPE ms)
{
    HAL_Delay((uint32_t)ms);
}

/* Initialize the sensor for accelerometer and gyroscope mode. */
s8 bno055_init_accgyro(void)
{
	s8 ret = BNO055_SUCCESS;

	bno055.dev_addr = BNO055_I2C_ADDR1;
	bno055.bus_read = bno055_i2c_read;
	bno055.bus_write = bno055_i2c_write;
	bno055.delay_msec = bno055_delay_ms;

	ret = bno055_init(&bno055);
	if (ret != BNO055_SUCCESS)
		return ret;

	/* TODO Put these in a config file. */
	ret |= bno055_set_accel_range(BNO055_ACCEL_RANGE_8G);
	ret |= bno055_set_accel_bw(BNO055_ACCEL_BW_250HZ);
	ret |= bno055_set_gyro_range(BNO055_GYRO_RANGE_2000DPS);
	ret |= bno055_set_gyro_bw(BNO055_GYRO_BW_32HZ);
	if (ret != BNO055_SUCCESS)
		return ret;

	return bno055_set_operation_mode(BNO055_OPERATION_MODE_ACCGYRO);
}

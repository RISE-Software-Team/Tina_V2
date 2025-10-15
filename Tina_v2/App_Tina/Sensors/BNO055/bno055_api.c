/*
	* bno055_api.c
	*
	*  Created on: Oct 13, 2025
	*      Author: krissal1234
	*/

#include "types_support.h"
#include "bno055_api.h"

#define BNO_I2C_TIMEOUT_MS 100

// Global BNO055 sensor structure
struct bno055_t bno055;

// Initialize BNO055 sensor (accelerometer and gyroscope mode)
MessageCode_t bno055_init_accgyro(void) {
	s8 result;
	
	// Set up I2C communication functions
	bno055.dev_addr = BNO055_I2C_ADDR1;
	bno055.bus_read = BNO055_I2C_bus_read;
	bno055.bus_write = BNO055_I2C_bus_write;
	bno055.delay_msec = BNO055_delay_msek;
	
	// Initialize the BNO055 driver
	result = bno055_init(&bno055);
	if (result != BNO055_SUCCESS) {
		return ERR_IMU_INIT_FAIL;
	}
	
	// Configure accelerometer: 8G range, 250Hz bandwidth
	result = bno055_set_accel_range(BNO055_ACCEL_RANGE_8G);
	if (result != BNO055_SUCCESS) {
		return ERR_IMU_INIT_FAIL;
	}
	
	result = bno055_set_accel_bw(BNO055_ACCEL_BW_250HZ);
	if (result != BNO055_SUCCESS) {
		return ERR_IMU_INIT_FAIL;
	}
	
	// Configure gyroscope: 2000 DPS range, 32Hz bandwidth
	result = bno055_set_gyro_range(BNO055_GYRO_RANGE_2000DPS);
	if (result != BNO055_SUCCESS) {
		return ERR_IMU_INIT_FAIL;
	}
	
	result = bno055_set_gyro_bw(BNO055_GYRO_BW_32HZ);
	if (result != BNO055_SUCCESS) {
		return ERR_IMU_INIT_FAIL;
	}
	
	// Set operation mode to accelerometer + gyroscope
	result = bno055_set_operation_mode(BNO055_OPERATION_MODE_ACCGYRO);
	if (result != BNO055_SUCCESS) {
		return ERR_IMU_INIT_FAIL;
	}
	
	return INFO_COMPONENT_SANITY_CHECK_PASS;
}

// Read acceleration data (X, Y, Z axes)
MessageCode_t BNO055_ReadAccel(struct bno055_accel_t *accel) {
	s8 result;
	
	// Null pointer check
	if (!accel) {
		return ERR_IMU_FAIL;
	}
	
	// Read acceleration data from sensor
	result = bno055_read_accel_xyz(accel);
	
	if (result != BNO055_SUCCESS) {
		return ERR_IMU_FAIL;
	}
	
	return INFO_COMPONENT_SANITY_CHECK_PASS;
}

// I2C read function for BNO055 driver
s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
	u16 hal_addr = (u16)(dev_addr << 1);
	HAL_StatusTypeDef status;
	
	status = HAL_I2C_Mem_Read(&hi2c2,
								hal_addr,
								(u16)reg_addr,
								I2C_MEMADD_SIZE_8BIT,
								reg_data,
								(u16)cnt,
								BNO_I2C_TIMEOUT_MS);
	
	return (status == HAL_OK) ? BNO055_SUCCESS : BNO055_ERROR;
}

// I2C write function for BNO055 driver
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
	u16 hal_addr = (u16)(dev_addr << 1);
	HAL_StatusTypeDef status;
	
	status = HAL_I2C_Mem_Write(&hi2c2,
								hal_addr,
								(u16)reg_addr,
								I2C_MEMADD_SIZE_8BIT,
								reg_data,
								(u16)cnt,
								BNO_I2C_TIMEOUT_MS);
	
	return (status == HAL_OK) ? BNO055_SUCCESS : BNO055_ERROR;
}

// Delay function for BNO055 driver
void BNO055_delay_msek(u32 msek) {
	HAL_Delay(msek);
}

/* OLD CODE - COMMENTED OUT FOR REFERENCE
	*
	* #include "main.h"
	* #include "bno055_support.h"
	*
	* #define BNO_I2C_TIMEOUT_MS  500
	*
	* extern I2C_HandleTypeDef hi2c2;
	*
	* struct bno055_t bno055;
	*
	* static s8 bno055_i2c_read(u8 dev_addr, u8 reg_addr, u8 *data, u8 len)
	* {
	*     uint16_t hal_addr = (uint16_t)(dev_addr << 1);
	*     HAL_StatusTypeDef rc = HAL_I2C_Mem_Read(&hi2c2,
	*                                            hal_addr,
	*                                            (uint16_t)reg_addr,
	*                                            I2C_MEMADD_SIZE_8BIT,
	*                                            data,
	*                                            (uint16_t)len,
	*                                            BNO_I2C_TIMEOUT_MS);
	*     return (rc == HAL_OK) ? BNO055_SUCCESS : BNO055_ERROR;
	* }
	*
	* static s8 bno055_i2c_write(u8 dev_addr, u8 reg_addr, u8 *data, u8 len)
	* {
	*     uint16_t hal_addr = (uint16_t)(dev_addr << 1);
	*     HAL_StatusTypeDef rc = HAL_I2C_Mem_Write(&hi2c2,
	*                                             hal_addr,
	*                                             (uint16_t)reg_addr,
	*                                             I2C_MEMADD_SIZE_8BIT,
	*                                             data,
	*                                             (uint16_t)len,
	*                                             BNO_I2C_TIMEOUT_MS);
	*     return (rc == HAL_OK) ? BNO055_SUCCESS : BNO055_ERROR;
	* }
	*
	* static void bno055_delay_ms(BNO055_MDELAY_DATA_TYPE ms)
	* {
	*     HAL_Delay((uint32_t)ms);
	* }
	*
	* s8 bno055_init_accgyro(void)
	* {
	*     s8 ret = BNO055_SUCCESS;
	*
	*     bno055.dev_addr = BNO055_I2C_ADDR1;
	*     bno055.bus_read = bno055_i2c_read;
	*     bno055.bus_write = bno055_i2c_write;
	*     bno055.delay_msec = bno055_delay_ms;
	*
	*     ret = bno055_init(&bno055);
	*     if (ret != BNO055_SUCCESS)
	*         return ret;
	*
	*     ret |= bno055_set_accel_range(BNO055_ACCEL_RANGE_8G);
	*     ret |= bno055_set_accel_bw(BNO055_ACCEL_BW_250HZ);
	*     ret |= bno055_set_gyro_range(BNO055_GYRO_RANGE_2000DPS);
	*     ret |= bno055_set_gyro_bw(BNO055_GYRO_BW_32HZ);
	*     if (ret != BNO055_SUCCESS)
	*         return ret;
	*
	*     return bno055_set_operation_mode(BNO055_OPERATION_MODE_ACCGYRO);
	* }
	*/

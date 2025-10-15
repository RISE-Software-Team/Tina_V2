/*
 * bno055_api.h
 *
 *  Created on: Oct 13, 2025
 *      Author: krissal1234
 */

#ifndef BNO055_API_H
#define BNO055_API_H

#include "Drivers/bno055.h"
#include "main.h"
#include "types_support.h"
#include "logger.h"

// External I2C handle (defined in main.c or i2c.c)
extern I2C_HandleTypeDef hi2c2;

// Global BNO055 sensor structure
extern struct bno055_t bno055;

// Initialize BNO055 sensor (accelerometer and gyroscope)
MessageCode_t bno055_init_accgyro(void);

// Read acceleration data (X, Y, Z axes)
MessageCode_t BNO055_ReadAccel(struct bno055_accel_t *accel);

// Driver interface functions (used internally by bno055.c driver)
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void BNO055_delay_msek(u32 msek);

#endif /* BME280_API_H */

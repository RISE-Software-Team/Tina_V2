/*
 * bno055_api.c
 *
 *  Created on: Oct 22, 2025
 *      Author: pranavsuri4303
 */

#include "bno055_api.h"

// Default hardware driver (defined in bme280_hw_driver.c)
extern const BNO055_Driver_t bno055_default_driver;

// Pointer to the currently registered driver (mock or hardware)
static const BNO055_Driver_t *driver = &bno055_default_driver;

/* -------------------------------------------------------------------------- */
/* Driver registration and retrieval                                          */
/* -------------------------------------------------------------------------- */
void BNO055_RegisterDriver(const BNO055_Driver_t *new_driver)
{
	if (new_driver)
		driver = new_driver;
}

/* -------------------------------------------------------------------------- */
/* Public wrapper API (calls into registered driver)                          */
/* -------------------------------------------------------------------------- */
int8_t BNO055_Init(void)
{
    return driver->init ? driver->init() : -1;
}

int BNO055_ReadAccel(BNO055_AccelData_t *accel)
{
    return driver->read_accel ? driver->read_accel(accel) : -1;
}

int BNO055_ReadGyro(BNO055_GyroData_t *gyro)
{
    return driver->read_gyro ? driver->read_gyro(gyro) : -1;
}

int BNO055_ReadEuler(BNO055_EulerData_t *euler)
{
    return driver->read_euler ? driver->read_euler(euler) : -1;
}

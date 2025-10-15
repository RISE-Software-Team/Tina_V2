/*
 * bno055_api.c
 *
 *  Created on: Oct 22, 2025
 *      Author: pranavsuri4303
 */

#include "types_support.h"
#include "bno055_api.h"

// Pointer to the currently registered driver (mock or hardware)
static const BNO055_Driver_t *registered_driver = NULL;

// Default hardware driver (defined in bme280_hw_driver.c)
extern const BNO055_Driver_t bno055_default_driver;

/* -------------------------------------------------------------------------- */
/* Driver registration and retrieval                                          */
/* -------------------------------------------------------------------------- */
void BNO055_RegisterDriver(const BNO055_Driver_t *driver)
{
    registered_driver = (driver != NULL) ? driver : &bno055_default_driver;
}

const BNO055_Driver_t *BNO055_GetRegisteredDriver(void)
{
    return registered_driver ? registered_driver : &bno055_default_driver;
}

/* -------------------------------------------------------------------------- */
/* Public wrapper API (calls into registered driver)                          */
/* -------------------------------------------------------------------------- */
int8_t BNO055_Init(void)
{
    const BNO055_Driver_t *drv = BNO055_GetRegisteredDriver();
    return drv->init ? drv->init() : -1;
}

int BNO055_ReadAccel(BNO055_AccelData_t *accel)
{
    const BNO055_Driver_t *drv = BNO055_GetRegisteredDriver();
    return drv->read_accel ? drv->read_accel(accel) : -1;
}

int BNO055_ReadGyro(BNO055_GyroData_t *gyro)
{
    const BNO055_Driver_t *drv = BNO055_GetRegisteredDriver();
    return drv->read_gyro ? drv->read_gyro(gyro) : -1;
}

int BNO055_ReadEuler(BNO055_EulerData_t *euler)
{
    const BNO055_Driver_t *drv = BNO055_GetRegisteredDriver();
    return drv->read_euler ? drv->read_euler(euler) : -1;
}

/*
 * bno055_api.h
 *
 *  Created on: Oct 22, 2025
 *      Author: pranavsuri4303
 */

#ifndef BNO055_API_H
#define BNO055_API_H

#include "bno055_api_types.h"
#include <stdint.h> 
#include <stddef.h>

// --------------------------------------------------------
// Driver interface (for mocking or hardware)
// --------------------------------------------------------
typedef struct {
    int8_t (*init)(void);
    int (*read_accel)(BNO055_AccelData_t *accel);
    int (*read_gyro)(BNO055_GyroData_t *gyro);
} BNO055_Driver_t;

// --------------------------------------------------------
// API functions
// --------------------------------------------------------
void BNO055_RegisterDriver(const BNO055_Driver_t *driver);

int8_t BNO055_Init(void);
int BNO055_ReadAccel(BNO055_AccelData_t *accel);
int BNO055_ReadGyro(BNO055_GyroData_t *gyro);

#endif /* BNO055_API_H */

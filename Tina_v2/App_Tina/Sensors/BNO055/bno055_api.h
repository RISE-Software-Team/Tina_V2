/*
 * bno055_api.h
 *
 *  Created on: Oct 22, 2025
 *      Author: pranavsuri4303
 */

#ifndef BNO055_API_H
#define BNO055_API_H

#include "bno055_api_types.h"
#include "main.h"
#include <stdint.h> 

// --------------------------------------------------------
// External dependencies
// --------------------------------------------------------
extern I2C_HandleTypeDef hi2c2;
extern struct bno055_t bno055;

// --------------------------------------------------------
// Driver interface (for mocking or hardware)
// --------------------------------------------------------
typedef struct {
    int8_t (*init)(void);
    int (*read_accel)(BNO055_AccelData_t *accel);
    int (*read_gyro)(BNO055_GyroData_t *gyro);
    int (*read_euler)(BNO055_EulerData_t *euler);
} BNO055_Driver_t;

// --------------------------------------------------------
// API functions
// --------------------------------------------------------
void BNO055_RegisterDriver(const BNO055_Driver_t *driver);
const BNO055_Driver_t *BNO055_GetRegisteredDriver(void);

int8_t BNO055_Init(void);
int BNO055_ReadAccel(BNO055_AccelData_t *accel);
int BNO055_ReadGyro(BNO055_GyroData_t *gyro);
int BNO055_ReadEuler(BNO055_EulerData_t *euler);

#endif /* BNO055_API_H */

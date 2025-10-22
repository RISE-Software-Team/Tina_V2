/*
 * bno055_api_types.h
 *
 *  Created on: Oct 22, 2025
 *      Author: pranavsuri4303
 */

#ifndef BNO055_API_TYPES_H
#define BNO055_API_TYPES_H

typedef struct {
    float x;  // m/s^2
    float y;
    float z;
} BNO055_AccelData_t;

typedef struct {
    float x;  // deg/s
    float y;
    float z;
} BNO055_GyroData_t;

typedef struct {
    float heading;  // degrees
    float roll;
    float pitch;
} BNO055_EulerData_t;

#endif /* BNO055_API_TYPES_H */

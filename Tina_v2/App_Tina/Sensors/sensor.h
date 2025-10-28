/*
 * sensors.h
 *
 *  Created on: Oct 15, 2025
 *      Author: iancu
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include <stdint.h>

typedef struct {
    float acc_x, acc_y, acc_z;
    float gyro_x, gyro_y, gyro_z;
    float pressure;
} SensorData_t;

uint8_t sensors_read_all(SensorData_t *data);

#endif /* SENSOR_H_ */

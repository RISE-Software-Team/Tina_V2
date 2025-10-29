#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include "sensor.h"

#include <stdint.h>


void telemetry_send(SensorData_t sensor_data, uint8_t sys_state);

#endif /* TELEMETRY_H_ */

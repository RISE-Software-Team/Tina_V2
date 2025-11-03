#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include "sensor.h"

#include "flight_fsm.h"

#include <stdint.h>

void telemetry_send(FlightFSM_t *fsm);

#endif /* TELEMETRY_H_ */

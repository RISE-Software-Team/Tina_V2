#ifndef FLIGHT_FSM_H
#define FLIGHT_FSM_H

#include "config.h"
#include "sensor.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    FLIGHT_STATE_PREFLIGHT = 0,
    FLIGHT_STATE_POWERED_ASCENT,
    FLIGHT_STATE_DROGUE_DESCENT,
    FLIGHT_STATE_MAIN_DESCENT,
    FLIGHT_STATE_RECOVERY,
    FLIGHT_STATE_ERROR
} FlightState_t;

typedef struct {
    bool imu_ok;
    bool barometer_ok;
    bool pyro_armed;
    bool drogue_fired;
    bool main_fired;
} SystemStatus_t;

typedef struct FlightFSM_t {
    FlightState_t state;
    SystemStatus_t status;
    SensorData_t sensor_data;

    void (*handler)(struct FlightFSM_t *fsm);

    float ground_pressure_pa;
    float max_pressure_pa;

    uint8_t pressure_index;
    float pressure_history[PRESSURE_HISTORY_SIZE];
} FlightFSM_t;

const char *flight_fsm_get_state_name(FlightState_t state);
void flight_fsm_init(FlightFSM_t *fsm);
void flight_fsm_compute_altitude(FlightFSM_t *fsm);
void flight_fsm_update(FlightFSM_t *fsm);

#endif /* FLIGHT_FSM_H */

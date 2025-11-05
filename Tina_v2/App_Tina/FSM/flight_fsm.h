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
    FLIGHT_STATE_ERROR
} FlightState_t;

typedef struct {
    bool imu_ok;
    bool baro_ok;
    bool pyro_armed;
    bool drogue_fired;
    bool main_fired;
} SystemStatus_t;

typedef struct {
    uint8_t pres_index;
    float pres[HISTORY_SIZE];
    float avg_pres;
    float avg_alt;
    uint8_t vert_acc_index;
    float vert_acc[HISTORY_SIZE];
    float avg_vert_acc;
} FlightHistory_t;

typedef struct FlightFSM_t {
    FlightState_t state;
    SystemStatus_t status;
    SensorData_t sensor_data;
    FlightHistory_t hist;

    void (*handler)(struct FlightFSM_t *fsm);

    uint32_t state_entry_time_ms;

    float ground_temp_k;
    float ground_pres_pa;

    float min_pres_pa;
} FlightFSM_t;

const char *flight_fsm_get_state_name(FlightState_t state);
void flight_fsm_init(FlightFSM_t *fsm, bool pyro_armed);
void flight_fsm_update(FlightFSM_t *fsm);

#endif /* FLIGHT_FSM_H */

/*
 * logger.h
 *
 *  Created on: Oct 7, 2025
 *      Author: pranavsuri4303
 */

#ifndef FLIGHT_FSM_H
#define FLIGHT_FSM_H

#include "types_support.h"
#include "logger.h"
#include "global_config.h"
#include "stm32wlxx_hal.h"

typedef enum {
    FLIGHT_STATE_PREFLIGHT = 0,
    FLIGHT_STATE_POWERED_ASCENT,
    FLIGHT_STATE_DROGUE_DESCENT,
    FLIGHT_STATE_MAIN_DESCENT,
    FLIGHT_STATE_RECOVERY,
    FLIGHT_STATE_ERROR
} FlightState_t;

typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float accel_magnitude;
    float vertical_velocity;
    float altitude;
    float temperature;
    float pressure;
    u32 timestamp_ms;
} SensorData_t;

typedef struct {
    bool imu_ok;
    bool barometer_ok;
    bool lora_ok;
    bool pyro_armed;
    bool drogue_fired;
    bool main_fired;
} SystemStatus_t;

typedef struct {
    u32 launch_time_ms;
    u32 apogee_time_ms;
    u32 drogue_deploy_time_ms;
    u32 main_deploy_time_ms;
    u32 touchdown_time_ms;
    float max_altitude_m;
    float max_acceleration_g;
} FlightLog_t;

typedef struct {
    FlightState_t current_state;
    FlightState_t previous_state;
    SystemStatus_t status;
    SensorData_t sensor_data;
    FlightLog_t flight_log;

    u32 state_entry_time_ms;
    u8 apogee_confirm_count;
    u8 touchdown_confirm_count;

    // Simple altitude history for apogee detection
    float prev_altitude;
    float prev_prev_altitude;

    float ground_altitude_m;
    bool ground_altitude_set;

    float ground_pressure_pa;
} FlightFSM_t;

// Init and update
MessageCode_t flight_fsm_init(FlightFSM_t *fsm);
void flight_fsm_update_sensors(FlightFSM_t *fsm, float accel_x, float accel_y,
                                float accel_z, float temperature, float pressure);
void flight_fsm_update(FlightFSM_t *fsm);

// State info
FlightState_t flight_fsm_get_state(FlightFSM_t *fsm);
const char* flight_fsm_get_state_name(FlightState_t state);

// State handlers
void flight_fsm_preflight_handler(FlightFSM_t *fsm);
void flight_fsm_powered_ascent_handler(FlightFSM_t *fsm);
void flight_fsm_drogue_descent_handler(FlightFSM_t *fsm);
void flight_fsm_main_descent_handler(FlightFSM_t *fsm);
void flight_fsm_recovery_handler(FlightFSM_t *fsm);
void flight_fsm_error_handler(FlightFSM_t *fsm);

// Detection helpers
bool flight_fsm_check_launch_detected(FlightFSM_t *fsm);
bool flight_fsm_check_apogee_detected(FlightFSM_t *fsm);
bool flight_fsm_check_main_altitude_reached(FlightFSM_t *fsm);
bool flight_fsm_check_touchdown_detected(FlightFSM_t *fsm);

#endif /* FLIGHT_FSM_H */
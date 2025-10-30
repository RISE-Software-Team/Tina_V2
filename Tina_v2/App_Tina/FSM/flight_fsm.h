#ifndef FLIGHT_FSM_H
#define FLIGHT_FSM_H

#include "../Utils/logger.h"
#include "../Utils/config.h"
#include <stdbool.h>
#include <stdint.h>

#define ALT_HISTORY_LEN 5

// -----------------------------------------------------------------------------
// Flight State Enumeration
// -----------------------------------------------------------------------------
typedef enum {
    FLIGHT_STATE_PREFLIGHT = 0,
    FLIGHT_STATE_POWERED_ASCENT,
    FLIGHT_STATE_DROGUE_DESCENT,
    FLIGHT_STATE_MAIN_DESCENT,
    FLIGHT_STATE_RECOVERY,
    FLIGHT_STATE_ERROR
} FlightState_t;

// -----------------------------------------------------------------------------
// Sensor Data Structure
// -----------------------------------------------------------------------------
typedef struct {
    float accel_x;              // m/s²
    float accel_y;              // m/s²
    float accel_z;              // m/s²
    float altitude;             // m (MSL)
    float pressure;             // hPa
} SensorData_t;

// -----------------------------------------------------------------------------
// System Status Structure
// -----------------------------------------------------------------------------
typedef struct {
    bool imu_ok;
    bool barometer_ok;
    bool lora_ok;
    bool pyro_armed;
    bool drogue_fired;
    bool main_fired;
} SystemStatus_t;

// -----------------------------------------------------------------------------
// Main FSM Structure
// -----------------------------------------------------------------------------
typedef struct {
    FlightState_t state;
    SystemStatus_t status;
    SensorData_t sensor_data;

    float max_altitude_m;
    
    uint32_t state_entry_time_ms;
    uint8_t apogee_confirm_count;
    uint8_t touchdown_confirm_count;
    
    float altitude_history[ALT_HISTORY_LEN];
    uint8_t altitude_history_index;
    
    float ground_altitude_m;
    bool ground_altitude_set;
} FlightFSM_t;

// -----------------------------------------------------------------------------
// Public API Functions
// -----------------------------------------------------------------------------

MessageCode_t flight_fsm_init(FlightFSM_t *fsm);

void flight_fsm_update_sensors(FlightFSM_t *fsm,
                                float accel_x, float accel_y, float accel_z,
                                float altitude, float pressure);

void flight_fsm_update(FlightFSM_t *fsm);

const char *flight_fsm_get_state_name(FlightState_t state);

// -----------------------------------------------------------------------------
// Detection Functions
// -----------------------------------------------------------------------------
bool flight_fsm_check_launch_detected(FlightFSM_t *fsm);
bool flight_fsm_check_apogee_detected(FlightFSM_t *fsm);
bool flight_fsm_check_main_altitude_reached(FlightFSM_t *fsm);
bool flight_fsm_check_touchdown_detected(FlightFSM_t *fsm);

#endif /* FLIGHT_FSM_H */

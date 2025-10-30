#include "flight_fsm.h"
#include "config.h"
#include "logger.h"
#include <string.h>
#include <math.h>

// -----------------------------------------------------------------------------
// Private Helper Functions
// -----------------------------------------------------------------------------

static void transition_state(FlightFSM_t *fsm, FlightState_t new_state)
{
    if (fsm->current_state == new_state) {
        return;
    }
    
    fsm->previous_state = fsm->current_state;
    fsm->current_state = new_state;
    fsm->state_entry_time_ms = 0; // TODO: Get actual timestamp using HAL_GetTick() or similar
    
    // Log state transition
    tlog(INFO_DEBUG, "State transition");
}

// -----------------------------------------------------------------------------
// State Handler Functions
// -----------------------------------------------------------------------------

/**
This function runs before launch to establish the ground altitude reference
and continuously monitors sensor data to detect launch conditions.
Once launch is detected (based on acceleration or altitude thresholds),
the FSM transitions to the Powered Ascent state.
 */
static void handle_preflight(FlightFSM_t *fsm)
{
    // Assumes sanity check of sensors and other systems has already passed.
    // Set ground altitude reference on first valid reading
    if (!fsm->ground_altitude_set) {
        fsm->ground_altitude_m = fsm->sensor_data.altitude;
        fsm->ground_altitude_set = true;
        tlog(INFO_ENTERED_PREFLIGHT_STAGE, "Ground altitude set");
    }
    
    // Check for launch
    if (flight_fsm_check_launch_detected(fsm)) {
        fsm->flight_log.launch_time_ms = 0; // TODO: Get actual timestamp using HAL_GetTick() or similar
        transition_state(fsm, FLIGHT_STATE_POWERED_ASCENT);
    }
}

/**
During this phase, the FSM tracks the rocket’s climb after launch.
It continuously updates the maximum recorded altitude and linear acceleration
(in m/s²), and monitors for apogee detection (when ascent ends and descent begins).
Once apogee is confirmed, the FSM transitions to the Drogue Descent state.
*/
static void handle_powered_ascent(FlightFSM_t *fsm)
{
    // Update max altitude
    if (fsm->sensor_data.altitude > fsm->flight_log.max_altitude_m) {
        fsm->flight_log.max_altitude_m = fsm->sensor_data.altitude;
    }
    
    float accel_ms2 = fsm->sensor_data.accel_magnitude;
    if (accel_ms2 > fsm->flight_log.max_acceleration_ms2) {
        fsm->flight_log.max_acceleration_ms2 = accel_ms2;
    }
    
    // Check for apogee
    if (flight_fsm_check_apogee_detected(fsm)) {
        fsm->flight_log.apogee_time_ms = 0; // TODO: Get actual timestamp using HAL_GetTick() or similar
        transition_state(fsm, FLIGHT_STATE_DROGUE_DESCENT);
    }
}

// TODO: verify if actual chute is released. using sensor values
// same for the drogue chute.
// 
// TODO: Look into stuff for backup charge.
// TDOO: Time based deployment in case of sensor failure.

static void handle_drogue_descent(FlightFSM_t *fsm)
{
    // Fire drogue on entry
    if (!fsm->status.drogue_fired) {
        // TODO: Fire drogue pyro
        fsm->status.drogue_fired = true;
        fsm->flight_log.drogue_deploy_time_ms = 0; // TODO: Get actual timestamp using HAL_GetTick() or similar
        tlog(INFO_DROGUE_PARACHUTE_DEPLOYED, "Drogue fired");
    }
    
    // Check for main deployment altitude
    if (flight_fsm_check_main_altitude_reached(fsm)) {
        transition_state(fsm, FLIGHT_STATE_MAIN_DESCENT);
    }
}

static void handle_main_descent(FlightFSM_t *fsm)
{
    // Fire main on entry
    if (!fsm->status.main_fired) {
        // TODO: Fire main pyro
        fsm->status.main_fired = true;
        fsm->flight_log.main_deploy_time_ms = 0; // TODO: Get actual timestamp using HAL_GetTick() or similar
        tlog(INFO_MAIN_PARACHUTE_DEPLOYED, "Main fired");
    }

    
    // Check for touchdown
    if (flight_fsm_check_touchdown_detected(fsm)) {
        fsm->flight_log.touchdown_time_ms = 0; // TODO: Get actual timestamp using HAL_GetTick() or similar
        transition_state(fsm, FLIGHT_STATE_RECOVERY);
    }
}

static void handle_recovery(FlightFSM_t *fsm)
{
    // Disarm pyros for safety
    if (fsm->status.pyro_armed) {
        // TODO: Disarm pyros
        fsm->status.pyro_armed = false;
    }
    
    // Log flight summary periodically
    tlog(INFO_ENTERED_TOUCHDOWN_STAGE, "Recovery mode");
}

static void handle_error(FlightFSM_t *fsm)
{
    // Log error state
    tlog(ERR_LOGIC_FAIL, "FSM in error state");
}

// -----------------------------------------------------------------------------
// Public API Implementation
// -----------------------------------------------------------------------------

MessageCode_t flight_fsm_init(FlightFSM_t *fsm)
{
    if (!fsm) {
        return ERR_COMPONENT_SANITY_CHECK_FAIL;
    }

    // Clear entire FSM structure
    memset(fsm, 0, sizeof(FlightFSM_t));

    // --- Initialize FSM state ---
    fsm->current_state = FLIGHT_STATE_PREFLIGHT;
    fsm->previous_state = FLIGHT_STATE_PREFLIGHT;
    fsm->state_entry_time_ms =  0; // TODO: Get actual timestamp using HAL_GetTick() or similar

    // --- Initialize subsystem statuses ---
    fsm->status.imu_ok = false;
    fsm->status.barometer_ok = false;
    fsm->status.lora_ok = true;          // assume comms ok by default
    fsm->status.pyro_armed = false;
    fsm->status.drogue_fired = false;
    fsm->status.main_fired = false;
    
    // --- Initialize Flight log ---
    fsm->flight_log.launch_time_ms        = 0;
    fsm->flight_log.apogee_time_ms        = 0;
    fsm->flight_log.drogue_deploy_time_ms = 0;
    fsm->flight_log.main_deploy_time_ms   = 0;
    fsm->flight_log.touchdown_time_ms     = 0;
    fsm->flight_log.max_altitude_m        = 0.0f;
    fsm->flight_log.max_acceleration_ms2  = 0.0f;


    fsm->sensor_data.accel_x = 0.0f;
    fsm->sensor_data.accel_y = 0.0f;
    fsm->sensor_data.accel_z = 0.0f;
    fsm->sensor_data.accel_magnitude = 0.0f;
    fsm->sensor_data.altitude = 0.0f;
    fsm->sensor_data.pressure = 0.0f;
    fsm->sensor_data.timestamp_ms = 0;

    // --- Initialize altitude history buffer ---
    memset(fsm->altitude_history, 0, sizeof(fsm->altitude_history));
    fsm->altitude_history_index = 0;

    // --- Other fields ---
    fsm->apogee_confirm_count = 0;
    fsm->touchdown_confirm_count = 0;
    fsm->ground_altitude_m = 0.0f;
    fsm->ground_altitude_set = false;

    tlog(INFO_COMPONENT_SANITY_CHECK_PASS, "Flight FSM initialized");

    return INFO_COMPONENT_SANITY_CHECK_PASS;
}


void flight_fsm_update_sensors(FlightFSM_t *fsm,
                                float accel_x, float accel_y, float accel_z,
                                float altitude, float pressure)
{
    if (!fsm) {
        return;
    }
    
    // Store sensor readings
    fsm->sensor_data = (SensorData_t){
        .accel_x = accel_x,
        .accel_y = accel_y,
        .accel_z = accel_z,
        .altitude = altitude,
        .pressure = pressure,
        .timestamp_ms = 0  // TODO: replace with HAL_GetTick() or similar
    };

    
    // Calculate acceleration magnitude
    fsm->sensor_data.accel_magnitude = sqrtf(accel_x * accel_x + accel_y * accel_y + accel_z * accel_z);
    
    fsm->altitude_history[fsm->altitude_history_index] = altitude;
    fsm->altitude_history_index = (fsm->altitude_history_index + 1) % ALT_HISTORY_LEN;
}

void flight_fsm_update(FlightFSM_t *fsm)
{
    if (!fsm) {
        return;
    }
    
    // Execute current state handler
    switch (fsm->current_state) {
        case FLIGHT_STATE_PREFLIGHT:
            handle_preflight(fsm);
            break;
            
        case FLIGHT_STATE_POWERED_ASCENT:
            handle_powered_ascent(fsm);
            break;
            
        case FLIGHT_STATE_DROGUE_DESCENT:
            handle_drogue_descent(fsm);
            break;
            
        case FLIGHT_STATE_MAIN_DESCENT:
            handle_main_descent(fsm);
            break;
            
        case FLIGHT_STATE_RECOVERY:
            handle_recovery(fsm);
            break;
            
        case FLIGHT_STATE_ERROR:
            handle_error(fsm);
            break;
            
        default:
            transition_state(fsm, FLIGHT_STATE_ERROR);
            break;
    }
}

const char *flight_fsm_get_state_name(FlightState_t state)
{
    switch (state) {
        case FLIGHT_STATE_PREFLIGHT:        return "PREFLIGHT";
        case FLIGHT_STATE_POWERED_ASCENT:   return "POWERED_ASCENT";
        case FLIGHT_STATE_DROGUE_DESCENT:   return "DROGUE_DESCENT";
        case FLIGHT_STATE_MAIN_DESCENT:     return "MAIN_DESCENT";
        case FLIGHT_STATE_RECOVERY:         return "RECOVERY";
        case FLIGHT_STATE_ERROR:            return "ERROR";
        default:                            return "UNKNOWN";
    }
}

// -----------------------------------------------------------------------------
// Detection Functions Implementation
// -----------------------------------------------------------------------------

bool flight_fsm_check_launch_detected(FlightFSM_t *fsm)
{
    if (!fsm) {
        return false;
    }

    // Linear acceleration magnitude in m/s² (gravity-compensated)
    float accel_ms2 = fsm->sensor_data.accel_magnitude;

    // Altitude above ground level (AGL)
    float altitude_agl = fsm->sensor_data.altitude - fsm->ground_altitude_m;

    // Launch detected if:
    // - linear accel exceeds threshold (e.g., > 10 m/s²)
    // - OR altitude rises above threshold (e.g., > 10 m)
    return (accel_ms2 > LAUNCH_ACCEL_THRESHOLD_MS2) ||
           (altitude_agl > LAUNCH_ALTITUDE_THRESHOLD_M);
}


/**
This function estimates the altitude trend using recent altitude samples
(stored in a ring buffer) and cross-checks it with IMU vertical acceleration.
Apogee is confirmed once both sensors indicate the start of descent
for a few consecutive samples. If one sensor fails, the other is used as fallback.
 */
bool flight_fsm_check_apogee_detected(FlightFSM_t *fsm)
{
    if (!fsm) return false;
    if (!fsm->ground_altitude_set) return false;                 // Ensure valid reference
    if (fsm->altitude_history_index < ALT_HISTORY_LEN - 1) return false; // Wait until buffer fills

    // Compute average slope of altitude (m per sample)
    float total_slope = 0.0f;
    for (int i = 1; i < ALT_HISTORY_LEN; i++) {
        total_slope += fsm->altitude_history[i] - fsm->altitude_history[i - 1];
    }
    float avg_slope = total_slope / (ALT_HISTORY_LEN - 1);

    // Check if altitude trend is negative (descending)
    bool altitude_decreasing = avg_slope < -0.3f;                // Threshold tuned for descent detection

    // Check IMU vertical acceleration
    float accel_z = fsm->sensor_data.accel_z;                    // Linear accel, m/s²
    bool accel_falling = accel_z < -1.0f;                        // Negative = downward acceleration

    // Combine both sources for robust apogee detection
    bool apogee_detected = false;
    if (fsm->status.imu_ok && fsm->status.barometer_ok)
        apogee_detected = altitude_decreasing && accel_falling;  // High confidence path
    else
        apogee_detected = altitude_decreasing || accel_falling;  // Fallback if one sensor fails

    // Apply confirmation counter with safety clamp
    if (apogee_detected) {
        if (fsm->apogee_confirm_count < 255)
            fsm->apogee_confirm_count++;
    } else {
        fsm->apogee_confirm_count = 0;
    }

    // Return true if confirmed for consecutive samples
    return (fsm->apogee_confirm_count >= 5);
}

bool flight_fsm_check_main_altitude_reached(FlightFSM_t *fsm)
{
    if (!fsm) return false;
    if (!fsm->status.barometer_ok || !fsm->ground_altitude_set) return false;
    // Calculate altitude above ground level
    float altitude_agl = fsm->sensor_data.altitude - fsm->ground_altitude_m;
    
    // Main deployment triggered when below threshold
    return altitude_agl <= MAIN_DEPLOY_ALTITUDE;
}

/**
Touchdown is confirmed when acceleration remains near 9.81 m/s² (stationary on ground)
and altitude remains nearly constant over several consecutive samples.
Uses the altitude history buffer for robust detection.
 */
bool flight_fsm_check_touchdown_detected(FlightFSM_t *fsm)
{
    if (!fsm) return false;

    // Ensure valid data
    if (!fsm->ground_altitude_set)
        return false;

    // Check if total acceleration is stable around 9.81 m/s² (stationary)
    bool accel_stable = fabsf(fsm->sensor_data.accel_magnitude) < TOUCHDOWN_ACCEL_TOLERANCE_MS2;

    // Compute altitude variation across history buffer
    float min_alt = fsm->altitude_history[0];
    float max_alt = fsm->altitude_history[0];
    for (int i = 1; i < ALT_HISTORY_LEN; i++) {
        if (fsm->altitude_history[i] < min_alt) min_alt = fsm->altitude_history[i];
        if (fsm->altitude_history[i] > max_alt) max_alt = fsm->altitude_history[i];
    }
    bool altitude_stable = (max_alt - min_alt) < 1.0f;  // 1 m tolerance

    // Confirm touchdown if both stable for several readings
    if (accel_stable && altitude_stable) {
        if (fsm->touchdown_confirm_count < 255)
            fsm->touchdown_confirm_count++;
    } else {
        fsm->touchdown_confirm_count = 0;
    }

    // Touchdown confirmed after 5 stable samples
    return fsm->touchdown_confirm_count >= 5;
}
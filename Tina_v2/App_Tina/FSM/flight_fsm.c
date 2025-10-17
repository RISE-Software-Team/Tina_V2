/*
 * flight_fsm.c
 *
 *  Created on: Oct 13, 2025
 *      Author: pranavsuri4303
 */

#include "flight_fsm.h"
#include "pyro_manager.h"
#include "../Sensors/BME280/bme280_api.h"
#include "../Sensors/BNO055/bno055_api.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

// Helper function to calculate altitude from pressure
static float calculate_altitude_from_pressure(
    float pressure_pa, 
    float ground_pressure_pa) 
{
    // Barometric formula: h = 44330 * (1 - (P/P0)^0.1903)
    float ratio = pressure_pa / ground_pressure_pa;
    return 44330.0f * (1.0f - powf(ratio, 0.1903f));
}

// Helper function to transition states
static void transition_state(
    FlightFSM_t *fsm, 
    FlightState_t new_state) 
{
    if (fsm->current_state == new_state) {
        return;
    }
    
    fsm->previous_state = fsm->current_state;
    fsm->current_state = new_state;
    fsm->state_entry_time_ms = HAL_GetTick();
    
    // Log state transition
    char msg[64];
    snprintf(msg, sizeof(msg), "State: %s -> %s",
             flight_fsm_get_state_name(fsm->previous_state),
             flight_fsm_get_state_name(new_state));
    tlog(INFO_FSM_STATE_TRANSITION, msg);
}

// Initialize FSM
MessageCode_t flight_fsm_init(FlightFSM_t *fsm) 
{
    if (!fsm) {
        return ERR_FSM_INIT_FAIL;
    }
    
    // Zero out the structure
    memset(fsm, 0, sizeof(FlightFSM_t));
    
    // Set initial state
    fsm->current_state = FLIGHT_STATE_PREFLIGHT;
    fsm->previous_state = FLIGHT_STATE_PREFLIGHT;
    fsm->state_entry_time_ms = HAL_GetTick();
    
    // Initialize system status
    fsm->status.imu_ok = false;
    fsm->status.barometer_ok = false;
    fsm->status.lora_ok = true;
    fsm->status.pyro_armed = false;
    fsm->status.drogue_fired = false;
    fsm->status.main_fired = false;
    
    // Initialize altitude history
    fsm->prev_altitude = 0.0f;
    fsm->prev_prev_altitude = 0.0f;
    fsm->ground_altitude_set = false;
    
    tlog(INFO_ENTERED_PREFLIGHT_STAGE, "FSM initialized");
    
    return INFO_COMPONENT_SANITY_CHECK_PASS;
}

// Update sensor data
void flight_fsm_update_sensors(
    FlightFSM_t *fsm, 
    float accel_x, 
    float accel_y,
    float accel_z, 
    float temperature, 
    float pressure) 
{
    /*
        1. check if fsm exists, return if null
        2. store all sensor readings
        3. calc acceleration magnitude from x, y, z
        4. calc vertical velocity from altitude change over time
        5. update previous altitude and time for next calculation
    */

    u32 current_timestamp_ms = HAL_GetTick();

    if (!fsm) {
        return;
    }

    fsm->prev_prev_altitude = fsm->prev_altitude;
    fsm->prev_altitude = fsm->sensor_data.altitude;

    fsm->sensor_data.accel_x = accel_x;
    fsm->sensor_data.accel_y = accel_y;
    fsm->sensor_data.accel_z = accel_z;
    fsm->sensor_data.temperature = temperature;
    fsm->sensor_data.pressure = pressure;

    fsm->sensor_data.accel_magnitude = sqrtf(
        pow(accel_x, 2) + 
        pow(accel_y, 2) + 
        pow(accel_z, 2)
    );

    fsm->sensor_data.altitude = calculate_altitude_from_pressure(
        pressure, 
        fsm->ground_pressure_pa
    );

    u32 dt = (current_timestamp_ms - fsm->sensor_data.timestamp_ms) / 1000.0f;

    if (dt <= 0) {
        dt = 0.01f; // Prevent division by zero, assume small time step
    }

    fsm->sensor_data.vertical_velocity += (fsm->sensor_data.altitude - fsm->prev_altitude) / dt;
    fsm->sensor_data.timestamp_ms = current_timestamp_ms;

    if (fsm->flight_log.max_altitude_m < fsm->sensor_data.altitude) {
        fsm->flight_log.max_altitude_m = fsm->sensor_data.altitude;
    }

    if (fsm->flight_log.max_acceleration_g < fsm->sensor_data.accel_magnitude) {
        fsm->flight_log.max_acceleration_g = fsm->sensor_data.accel_magnitude;
    }

}

// Main FSM update function
void flight_fsm_update(FlightFSM_t *fsm) 
{
    if (!fsm) {
        return;
    }


    // read data from sensor api and update fsm sensors. Ideally reading from api should be done inside update function

    // flight_fsm_update_sensors(
    //     fsm,
    // );
    
    // Call appropriate state handler
    switch (fsm->current_state) {
        case FLIGHT_STATE_PREFLIGHT:
            flight_fsm_preflight_handler(fsm);
            break;
            
        case FLIGHT_STATE_POWERED_ASCENT:
            flight_fsm_powered_ascent_handler(fsm);
            break;
            
        case FLIGHT_STATE_DROGUE_DESCENT:
            flight_fsm_drogue_descent_handler(fsm);
            break;
            
        case FLIGHT_STATE_MAIN_DESCENT:
            flight_fsm_main_descent_handler(fsm);
            break;
            
        case FLIGHT_STATE_RECOVERY:
            flight_fsm_recovery_handler(fsm);
            break;
            
        case FLIGHT_STATE_ERROR:
            flight_fsm_error_handler(fsm);
            break;
            
        default:
            tlog(ERR_FSM_INVALID_STATE, "Invalid FSM state");
            transition_state(fsm, FLIGHT_STATE_ERROR);
            break;
    }
}

// PreFlight state handler
void flight_fsm_preflight_handler(FlightFSM_t *fsm) 
{
    static u32 last_status_report_ms = 0;
    u32 current_time = HAL_GetTick();
    u32 time_in_state = current_time - fsm->state_entry_time_ms;
    
    // Check for preflight timeout
    // Set ground altitude reference on first valid reading
    // check for launch detection

    /* 
        1. if fsm null return
        2. verify imu and barometer comms -> no need to check for comms since thats the first thing checked in main.c
        3. Verify LoRa telemetry link is operational -> same as above
        4. perform sensor checks -> same as above
        5. Report system status to telemetry
        6. If launch detected, transition to PoweredAscent

        Launch Detection Criteria:
            Acceleration > +3g 
            OR 
            Altitude > 10m
    */

    if (!fsm) {
        return;
    }

}

// PoweredAscent state handler
void flight_fsm_powered_ascent_handler(FlightFSM_t *fsm) 
{

    /*
        1. Increase telemetry rate and send data to ground station
        2. Check for apogee detection
        3. Update max altitude and acceleration
        4. Safety timeout check (e.g. if powered ascent lasts too long without apogee change state to next state)
    */
    u32 current_time = HAL_GetTick();

    if (fsm->sensor_data.altitude < fsm->flight_log.max_altitude_m) {
        fsm->altitude_decrease_count++;
    } else {
        fsm->altitude_decrease_count = 0;
    }

    if (fsm->sensor_data.vertical_velocity - APOGEE_VELOCITY_EPSILON < 0) {
        fsm->vertical_velocity_negative_count++;
    } else {
        fsm->vertical_velocity_negative_count = 0;
    }

    if (
        fsm->altitude_decrease_count >= APOGEE_SAMPLE_COUNT ||
        fsm->vertical_velocity_negative_count >= APOGEE_SAMPLE_COUNT ||
        current_time - fsm->flight_log.launch_time_ms > APOGEE_TIMEOUT_MS
    ) {
        fsm->flight_log.apogee_time_ms = current_time;
        transition_state(fsm, FLIGHT_STATE_DROGUE_DESCENT);
        
        // TO DO: Fire drogue parachute
    }

    // TODO: SEND TELEMETRY

}

// DrogueDescent state handler
void flight_fsm_drogue_descent_handler(FlightFSM_t *fsm) 
{
    u32 current_time = HAL_GetTick();
    
    // Fire drogue on entry
    // Check for main deployment altitude
}

// MainDescent state handler
void flight_fsm_main_descent_handler(FlightFSM_t *fsm) 
{
    u32 current_time = HAL_GetTick();
    
    // Fire main on entry
    // Check for touchdown
}

// Recovery state handler
void flight_fsm_recovery_handler(FlightFSM_t *fsm) 
{
    u32 current_time = HAL_GetTick();
    
    // Disarm pyros for safety
    // low-rate status reporting
}

// Error state handler
void flight_fsm_error_handler(FlightFSM_t *fsm) 
{
    static u32 last_error_report_ms = 0;
}

// Launch detection
bool flight_fsm_check_launch_detected(FlightFSM_t *fsm) 
{
    // calc acceleration in g's
    // calc altitude above ground
    // launch detected if: accel > 3g OR altitude > 10m
}

// Apogee detection
bool flight_fsm_check_apogee_detected(FlightFSM_t *fsm) 
{
    // TODO: Figure this out
    return false;
}

// Main altitude check
bool flight_fsm_check_main_altitude_reached(FlightFSM_t *fsm) 
{
    // TODO: Figure this out
    return false;
}

// Touchdown detection
bool flight_fsm_check_touchdown_detected(FlightFSM_t *fsm) 
{
    // TODO: Figure this out
    return false;
}

// Get current state
FlightState_t flight_fsm_get_state(FlightFSM_t *fsm) 
{
    return fsm ? fsm->current_state : FLIGHT_STATE_ERROR;
}

// Get state name string
const char* flight_fsm_get_state_name(FlightState_t state) 
{
    switch (state) {
        case FLIGHT_STATE_PREFLIGHT:
            return "PREFLIGHT";
        case FLIGHT_STATE_POWERED_ASCENT:
            return "POWERED_ASCENT";
        case FLIGHT_STATE_DROGUE_DESCENT:
            return "DROGUE_DESCENT";
        case FLIGHT_STATE_MAIN_DESCENT:
            return "MAIN_DESCENT";
        case FLIGHT_STATE_RECOVERY:
            return "RECOVERY";
        case FLIGHT_STATE_ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}
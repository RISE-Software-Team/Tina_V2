#include "flight_fsm.h"

#include "config.h"
#include "lang.h"
#include "logger.h"

#include <float.h>
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 ****************************** Private functions ******************************
 *******************************************************************************/
static void transition_state(FlightFSM_t *fsm, FlightState_t new_state);

/* Detection functions */
static bool flight_fsm_check_launch_detected(FlightFSM_t *fsm);
static bool flight_fsm_check_apogee_detected(FlightFSM_t *fsm);
static bool flight_fsm_check_main_altitude_reached(FlightFSM_t *fsm);

/* State handler functions */
static void handle_preflight(FlightFSM_t *fsm);
static void handle_powered_ascent(FlightFSM_t *fsm);
static void handle_drogue_descent(FlightFSM_t *fsm);
static void handle_main_descent(FlightFSM_t *fsm);
static void handle_error(FlightFSM_t *fsm);

/*******************************************************************************
 ******************************* Public functions ******************************
 *******************************************************************************/
const char *flight_fsm_get_state_name(FlightState_t state)
{
    switch (state) {
    case FLIGHT_STATE_PREFLIGHT:        return "PREFLIGHT";
    case FLIGHT_STATE_POWERED_ASCENT:   return "POWERED_ASCENT";
    case FLIGHT_STATE_DROGUE_DESCENT:   return "DROGUE_DESCENT";
    case FLIGHT_STATE_MAIN_DESCENT:     return "MAIN_DESCENT";
    case FLIGHT_STATE_ERROR:            return "ERROR";
    default:                            return "UNKNOWN";
    }
}

void flight_fsm_init(FlightFSM_t *fsm)
{
    if (!fsm)
        return;

    memset(fsm, 0, sizeof(FlightFSM_t));

    fsm->state = FLIGHT_STATE_PREFLIGHT;
    fsm->handler = handle_preflight;

    fsm->ground_pressure_pa = FLT_MAX;
    fsm->min_pressure_pa = FLT_MAX;

    tlog(INFO_COMPONENT_SANITY_CHECK_PASS, "Flight FSM initialized"); //TODO add log code
}

void flight_fsm_update(FlightFSM_t *fsm)
{
    if (!fsm || !fsm->handler)
        return;

    fsm->handler(fsm);
}

static void transition_state(FlightFSM_t *fsm, FlightState_t new_state)
{
    if (fsm->state == new_state)
        return;

    switch (new_state) {
    case FLIGHT_STATE_PREFLIGHT:
        fsm->handler = handle_preflight;
        break;
    case FLIGHT_STATE_POWERED_ASCENT:
        fsm->handler = handle_powered_ascent;
        break;
    case FLIGHT_STATE_DROGUE_DESCENT:
        fsm->handler = handle_drogue_descent;
        break;
    case FLIGHT_STATE_MAIN_DESCENT:
        fsm->handler = handle_main_descent;
        break;
    case FLIGHT_STATE_ERROR:
        fsm->handler = handle_error;
        break;
    default:
        return;
    }

    fsm->state = new_state;

    char message[MAX_LOG_MESSAGE_LEN];
    snprintf(message, sizeof(message), "Transition: %u -> %u", fsm->state, new_state);
    tlog(INFO_DEBUG, message); //TODO add different log code
}


static bool flight_fsm_check_launch_detected(FlightFSM_t *fsm)
{
    if (!fsm)
        return false;

    if (fsm->status.imu_ok && fsm->sensor_data.acc_x > LAUNCH_ACCEL_THRESHOLD_MS2)
    	return true;

    if (fsm->status.baro_ok && fsm->altitude_m > LAUNCH_ALTITUDE_THRESHOLD_M)
    	return true;

    /* What should we do in the case both imu and baro are failing? */

    return false;
}

static float compute_average_pressure_from_history(float *pressure_history)
{
	float avg_pressure = 0;
	for (uint8_t i = 0; i < PRESSURE_HISTORY_SIZE; i++)
		avg_pressure += pressure_history[i];
	return avg_pressure / PRESSURE_HISTORY_SIZE;
}

/* Apogee is detected if the average pressure from history is
 * higher than the minimum pressure. */
static bool flight_fsm_check_apogee_detected(FlightFSM_t *fsm)
{
    if (!fsm)
        return false;

    float avg_pressure = compute_average_pressure_from_history(fsm->pressure_history);
    bool apogee_detected = false;

    if (fsm->status.baro_ok && avg_pressure > fsm->min_pressure_pa)
    	apogee_detected = true;

    /* What should we do if the barometer failed? */

    static int8_t apogee_countdown = APOGEE_COUNTDOWN_SIZE;
    if (apogee_detected) {
    	apogee_countdown = min(apogee_countdown - 1, 0);
    } else {
    	apogee_countdown = APOGEE_COUNTDOWN_SIZE;
    }

    return apogee_countdown == 0;
}

static bool flight_fsm_check_main_altitude_reached(FlightFSM_t *fsm)
{
    if (!fsm)
    	return false;

    if (fsm->status.baro_ok && fsm->altitude_m < MAIN_DEPLOY_ALTITUDE_M)
    	return true;

    /* What should we do if the barometer failed? */

    return false;
}

static void handle_preflight(FlightFSM_t *fsm)
{
    if (!fsm)
        return;

    if (flight_fsm_check_launch_detected(fsm))
        transition_state(fsm, FLIGHT_STATE_POWERED_ASCENT);
}

static void handle_powered_ascent(FlightFSM_t *fsm)
{
    if(!fsm)
        return;

    if (flight_fsm_check_apogee_detected(fsm)) {

        /* Fire drogue */

        fsm->status.drogue_fired = true;
        tlog(INFO_DROGUE_PARACHUTE_DEPLOYED, "Drogue fired"); //TODO code for this

        transition_state(fsm, FLIGHT_STATE_DROGUE_DESCENT);
    }
}

// TODO: verify if actual chute is released. using sensor values
// same for the drogue chute.
// 
// TODO: Look into stuff for backup charge.
// TODO: Time based deployment in case of sensor failure.
static void handle_drogue_descent(FlightFSM_t *fsm)
{
    if (!fsm)
        return;

    if (flight_fsm_check_main_altitude_reached(fsm)) {

        /* Fire main */

        fsm->status.main_fired = true;
        tlog(INFO_DROGUE_PARACHUTE_DEPLOYED, "Main fired"); //TODO code for this

        transition_state(fsm, FLIGHT_STATE_MAIN_DESCENT);
    }
}

static void handle_main_descent(FlightFSM_t *fsm)
{
    /* Nothing to do */
}

static void handle_error(FlightFSM_t *fsm)
{
    tlog(ERR_LOGIC_FAIL, "FSM in error state"); //TODO code for this
}

#include "flight_fsm.h"

#include "config.h"
#include "lang.h"
#include "logger.h"
#include "telemetry.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define DIFF_C_K 273.15f

#define R_CONST 287.052874f
#define G_CONST 9.80665f
#define L_RATE  0.0065f
#define ALPHA ((R_CONST * L_RATE) / G_CONST)

/*******************************************************************************
 ****************************** Private functions ******************************
 *******************************************************************************/
static void transition_state(FlightFSM_t *fsm, FlightState_t new_state);
static float compute_average_value_from_history(float *hist);
static float compute_altitude(float pres, float ground_pres, float ground_temp);
static void update_data(FlightFSM_t *fsm);

/* Detection functions */
static bool launch_detected(FlightFSM_t *fsm);
static bool apogee_detected(FlightFSM_t *fsm);
static bool main_altitude_reached(FlightFSM_t *fsm);

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

    fsm->ground_temp_k = FLT_MAX;
    fsm->ground_pres_pa = FLT_MAX;
    fsm->min_pres_pa = FLT_MAX;

    tlog(INFO_FSM_INIT_PASS, NULL);
}

void flight_fsm_update(FlightFSM_t *fsm)
{
    if (!fsm || !fsm->handler)
        return;

    update_data(fsm);
    telemetry_send(fsm);
    fsm->handler(fsm);
}

static void transition_state(FlightFSM_t *fsm, FlightState_t new_state)
{
    if (fsm->state == new_state)
        return;

    int8_t log_code = INFO_DEBUG;

    switch (new_state) {
    case FLIGHT_STATE_PREFLIGHT:
        fsm->handler = handle_preflight;
        log_code = INFO_ENTERED_PREFLIGHT_STAGE;
        break;
    case FLIGHT_STATE_POWERED_ASCENT:
        fsm->handler = handle_powered_ascent;
        log_code = INFO_ENTERED_POWERED_ASCENT_STAGE;
        break;
    case FLIGHT_STATE_DROGUE_DESCENT:
        fsm->handler = handle_drogue_descent;
        log_code = INFO_ENTERED_DROGUE_DESCENT_STAGE;
        break;
    case FLIGHT_STATE_MAIN_DESCENT:
        fsm->handler = handle_main_descent;
        log_code = INFO_ENTERED_MAIN_DESCENT_STAGE;
        break;
    case FLIGHT_STATE_ERROR:
        fsm->handler = handle_error;
        log_code = ERR_FSM_STATE_FAIL;
        break;
    default:
        return;
    }

    char log_msg[MAX_LOG_MESSAGE_LEN];
    snprintf(log_msg, sizeof(log_msg), "State transition: %u -> %u", fsm->state, new_state);
    tlog(log_code, log_msg);

    fsm->state = new_state;
}

static float compute_average_value_from_history(float *hist)
{
	float avg_val = 0;
	for (uint8_t i = 0; i < HISTORY_SIZE; i++)
		avg_val += hist[i];
	return avg_val / HISTORY_SIZE;
}

static float compute_altitude(float pres, float ground_pres, float ground_temp)
{
    return (ground_temp / L_RATE) * (1.0 - pow(pres / ground_pres, ALPHA));
}

static void update_data(FlightFSM_t *fsm)
{
	if (!fsm)
		return;

    int8_t ret = sensors_read_all(&fsm->sensor_data);
    switch (ret) {
    case 0:
    	fsm->status.baro_ok = true;
    	fsm->status.imu_ok = true;
    	break;
    case -1:
    	fsm->status.baro_ok = false;
    	fsm->status.imu_ok = true;
    	break;
    case -2:
    	fsm->status.baro_ok = true;
    	fsm->status.imu_ok = false;
    	break;
    default:
    	fsm->status.baro_ok = false;
    	fsm->status.imu_ok = false;
    }

    if (fsm->status.baro_ok) {
        if (fsm->ground_temp_k == FLT_MAX) {
            fsm->ground_temp_k = fsm->sensor_data.temp + DIFF_C_K;
    		fsm->ground_pres_pa = fsm->sensor_data.pres;

            for (uint8_t i = 0; i < HISTORY_SIZE; i++)
                fsm->hist.pres[i] = fsm->sensor_data.pres;

            tlog(INFO_GROUND_PRES_AND_TEMP_SET, NULL);
    	}

        fsm->hist.pres[fsm->hist.pres_index] = fsm->sensor_data.pres;
        fsm->hist.pres_index = (fsm->hist.pres_index + 1) % HISTORY_SIZE;
        fsm->hist.avg_pres = compute_average_value_from_history(fsm->hist.pres);
        fsm->hist.avg_alt = compute_altitude(fsm->hist.avg_pres, fsm->ground_pres_pa, fsm->ground_temp_k);

        fsm->min_pres_pa = min(fsm->min_pres_pa, fsm->sensor_data.pres);
    }

    if (fsm->status.imu_ok) {
        fsm->hist.vert_acc[fsm->hist.vert_acc_index] = fsm->sensor_data.acc_z;
        fsm->hist.vert_acc_index = (fsm->hist.vert_acc_index + 1) % HISTORY_SIZE;
        fsm->hist.avg_vert_acc = compute_average_value_from_history(fsm->hist.vert_acc);
    }
}

static bool launch_detected(FlightFSM_t *fsm)
{
    if (!fsm)
        return false;

    if (fsm->status.imu_ok && fsm->hist.avg_vert_acc > LAUNCH_ACCEL_THRESHOLD_MS2)
    	return true;

    if (fsm->status.baro_ok && fsm->hist.avg_alt > LAUNCH_ALTITUDE_THRESHOLD_M)
    	return true;

    return false;
}

/* Apogee is detected if the average pressure from history is
 * higher than the minimum pressure. */
static bool apogee_detected(FlightFSM_t *fsm)
{
    if (!fsm)
        return false;

    bool apogee_detected = false;
    if (fsm->status.baro_ok && fsm->hist.avg_pres > fsm->min_pres_pa)
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

static bool main_altitude_reached(FlightFSM_t *fsm)
{
    if (!fsm)
    	return false;

    if (fsm->status.baro_ok && fsm->hist.avg_alt < MAIN_DEPLOY_ALTITUDE_M)
    	return true;

    /* What should we do if the barometer failed? */

    return false;
}

static void handle_preflight(FlightFSM_t *fsm)
{
    if (!fsm)
        return;

    if (launch_detected(fsm))
        transition_state(fsm, FLIGHT_STATE_POWERED_ASCENT);
}

static void handle_powered_ascent(FlightFSM_t *fsm)
{
    if(!fsm)
        return;

    if (apogee_detected(fsm)) {

        /* Fire drogue */

        fsm->status.drogue_fired = true;
        tlog(INFO_DROGUE_PARACHUTE_DEPLOYED, NULL);

        transition_state(fsm, FLIGHT_STATE_DROGUE_DESCENT);
    }
}

static void handle_drogue_descent(FlightFSM_t *fsm)
{
    if (!fsm)
        return;

    if (main_altitude_reached(fsm)) {

        /* Fire main */

        fsm->status.main_fired = true;
        tlog(INFO_MAIN_PARACHUTE_DEPLOYED, NULL);

        transition_state(fsm, FLIGHT_STATE_MAIN_DESCENT);
    }
}

static void handle_main_descent(FlightFSM_t *fsm)
{
    /* Nothing to do */
}

static void handle_error(FlightFSM_t *fsm)
{
    tlog(ERR_FSM_STATE_FAIL, NULL);
}

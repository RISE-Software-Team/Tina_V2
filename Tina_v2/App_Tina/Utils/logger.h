/*
 * error_handler.h
 *
 *  Created on: Oct 7, 2025
 *      Author: krissal1234
 */

#ifndef UTILS_LOGGER_H_
#define UTILS_LOGGER_H_

typedef enum {
    // ---------- INFO CODES (positive) ----------
    INFO_ENTERED_PREFLIGHT_STAGE          = 1,
    INFO_ENTERED_POWERED_ASCENT_STAGE     = 2,
    INFO_ENTERED_DROGUE_DESCENT_STAGE     = 3,
    INFO_ENTERED_MAIN_DESCENT_STAGE       = 4,
    INFO_ENTERED_TOUCHDOWN_STAGE          = 5,

    INFO_DROGUE_PARACHUTE_DEPLOYED        = 6,
    INFO_MAIN_PARACHUTE_DEPLOYED          = 7,
    INFO_COMPONENT_SANITY_CHECK_PASS      = 8,
	INFO_DEBUG							  = 9,

	INFO_LAUNCH_DETECTED                  = 10,  // Launch detected
	INFO_APOGEE_DETECTED                  = 11,  // Apogee detected
	INFO_MAIN_ALTITUDE_REACHED            = 12,  // Main deployment altitude reached
	INFO_TOUCHDOWN_DETECTED               = 13,  // Touchdown detected
	INFO_FSM_STATE_TRANSITION             = 14,  // General state transition
	INFO_GROUND_ALTITUDE_SET              = 15,  // Ground altitude reference set
	INFO_CONFIG_LOADED                    = 16,  // Configuration loaded from SD
	INFO_FLIGHT_SUMMARY                   = 17,  // Flight summary data

    // ---------- ERROR CODES (negative) ----------
    ERR_IMU_FAIL                          = -1,   // BNO055 failure
    ERR_IMU_INIT_FAIL                     = -2,
    ERR_IMU_CALIB_FAIL                    = -3,

    ERR_BARO_FAIL                         = -4,   // BME280 failure
    ERR_BARO_INIT_FAIL                    = -5,
    ERR_BARO_CALIB_FAIL                   = -6,

    ERR_PYRO_DROGUE_FAIL                  = -7,   // Pyro channel firing fault
    ERR_PYRO_MAIN_FAIL                    = -8,   // Pyro channel firing fault
    ERR_TIMEOUT_APOGEE                    = -9,   // Drogue deployment triggered by safety timeout

    ERR_MISC_ERR                          = -10,  // Undefined software exception
    ERR_LOGIC_FAIL                        = -11,  // FSM state went invalid
    ERR_COMPONENT_SANITY_CHECK_FAIL       = -12,

	ERR_DEBUG                             = -13,
	ERR_FSM_INIT_FAIL                     = -14,  // FSM initialization failed
	ERR_FSM_INVALID_STATE                 = -15,  // FSM entered invalid state
	ERR_PREFLIGHT_TIMEOUT                 = -16,  // Preflight stage timeout
	ERR_CONFIG_LOAD_FAIL                  = -17,  // Failed to load configuration
	ERR_SENSOR_UPDATE_FAIL                = -18   // Sensor data update failed
} MessageCode_t;


void tlog(MessageCode_t message_code, const char *msg);



#endif /* UTILS_LOGGER_H_ */

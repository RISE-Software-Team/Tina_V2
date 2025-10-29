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
	INFO_DEBUG                            = 0,

	INFO_COMPONENT_SANITY_CHECK_PASS      = 1,

	INFO_ENTERED_PREFLIGHT_STAGE          = 2,
	INFO_ENTERED_POWERED_ASCENT_STAGE     = 3,
	INFO_ENTERED_DROGUE_DESCENT_STAGE     = 4,
	INFO_ENTERED_MAIN_DESCENT_STAGE       = 5,
	INFO_ENTERED_TOUCHDOWN_STAGE          = 6,

	INFO_DROGUE_PARACHUTE_DEPLOYED        = 7,
	INFO_MAIN_PARACHUTE_DEPLOYED          = 8,

    // ---------- ERROR CODES (negative) ----------
	ERR_COMPONENT_SANITY_CHECK_FAIL       = -1,

	ERR_IMU_INIT_FAIL                     = -2,   // BNO055 failure
	ERR_IMU_CALIB_FAIL                    = -3,
	ERR_IMU_READ_ACCEL_FAIL               = -4,
	ERR_IMU_READ_GYRO_FAIL                = -5,

	ERR_BARO_INIT_FAIL                    = -6,   // BME280 failure
	ERR_BARO_CALIB_FAIL                   = -7,
	ERR_BARO_READ_PRESSURE_FAIL           = -8,

	ERR_PYRO_DROGUE_FAIL                  = -9,   // Pyro channel firing fault
	ERR_PYRO_MAIN_FAIL                    = -10,  // Pyro channel firing fault
	ERR_TIMEOUT_APOGEE                    = -11,  // Drogue deployment triggered by safety timeout

	ERR_MISC_ERR                          = -12,  // Undefined software exception
	ERR_LOGIC_FAIL                        = -13,  // FSM state went invalid

	ERR_TELEMETRY_SEND_FAIL               = -14,
} MessageCode_t;


void tlog(MessageCode_t message_code, const char *msg);

#endif /* UTILS_LOGGER_H_ */

/*
 * error_handler.h
 *
 *  Created on: Oct 7, 2025
 *      Author: krissal1234
 */

#ifndef UTILS_LOGGER_H_
#define UTILS_LOGGER_H_

typedef enum {
	//imu
    ERR_IMU_FAIL        = 0x01, // BNO055 failure
    ERR_IMU_INIT_FAIL       = 0x02,
    ERR_IMU_CALIB_FAIL      = 0x03,

	//barometer
    ERR_BARO_FAIL       = 0x04, // BME280 failure
    ERR_BARO_INIT_FAIL       = 0x05,
    ERR_BARO_CALIB_FAIL       = 0x06,

	//parachutes
    ERR_PYRO_DROGUE_FAIL       = 0x07, // Pyro channel firing fault
    ERR_PYRO_MAIN_FAIL       = 0x08, // Pyro channel firing fault
    ERR_TIMEOUT_APOGEE  = 0x09, // Drogue deployment triggered by safety timeout

	//other
    ERR_MISC_ERR        = 0x0A, // undefined software exception
    ERR_LOGIC_FAIL      = 0x0B, // FSM state has gone to impossible state
	ERR_COMPONENT_SANITY_CHECK_FAIL = 0x0C,
} ErrorCode_t;

typedef enum {
	//stages
    INFO_ENTERED_PREFLIGHT_STAGE = 0x01,
	INFO_ENTERED_POWERED_ASCENT_STAGE = 0x02,
	INFO_ENTERED_DROGUE_DESCENT_STAGE = 0x03,
	INFO_ENTERED_MAIN_DESCENT_STAGE = 0x04,
	INFO_ENTERED_TOUCHDOWN_STAGE = 0x05,

	//Parachute deployment
	INFO_DROGUE_PARACHUTE_DEPLOYED=0x06,
	INFO_MAIN_PARACHUTE_DEPLOYED = 0x07,

	INFO_COMPONENT_SANITY_CHECK_PASS = 0x08,
} InfoCode_t;


typedef enum {
    SEV_WARNING = 0x01,
    SEV_ERROR   = 0x02,
} Severity_t;

void ErrorHandler_Report(Severity_t type, ErrorCode_t code, const char *msg);
#endif /* UTILS_LOGGER_H_ */

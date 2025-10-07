/*
 * error_handler.h
 *
 *  Created on: Oct 7, 2025
 *      Author: krissal1234
 */

#ifndef UTILS_ERROR_HANDLER_H_
#define UTILS_ERROR_HANDLER_H_

typedef enum {
    ERR_IMU_FAIL        = 0x01, // BNO055 failure (no response or bad data)
    ERR_BARO_FAIL       = 0x02, // BME280 failure (no response or bad data)
    ERR_PYRO_FAIL       = 0x03, // Pyro channel firing fault
    ERR_MISC_ERR        = 0x04, // undefined software exception
    ERR_IMU_CALIB_ERR       = 0x05, // Sensor calibration data failed to load/read
    ERR_BARO_CALIB_ERR      = 0x06, // Sensor calibration data failed to load/read
    ERR_LOGIC_FAIL      = 0x07, // FSM state has gone to impossible state
    ERR_TIMEOUT_APOGEE  = 0x08, // Drogue deployment triggered by safety timeout
} ErrorCode_t;

typedef enum {
    SEV_WARNING = 0x01,
    SEV_ERROR   = 0x02,
} Severity_t;

void ErrorHandler_Report(Severity_t type, ErrorCode_t code, const char *msg);
#endif /* UTILS_ERROR_HANDLER_H_ */

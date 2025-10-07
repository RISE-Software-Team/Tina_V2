/*
 * packet.h
 *
 *  Created on: Oct 7, 2025
 *      Author: krissal1234
 */

#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include "error_handler.h"

typedef enum {
    PACKET_TYPE_TELEMETRY = 0x01,
    PACKET_TYPE_INFO      = 0x02,
    PACKET_TYPE_ERROR     = 0x03,
} PacketType_t;

typedef struct {
    int16_t acc_x, acc_y, acc_z;
    int16_t gyro_x, gyro_y, gyro_z;
    uint32_t altitude;
    uint8_t event_flags;
    uint8_t sys_state;
} TelemetryData_t;

typedef struct {
	Severity_t severity;
	ErrorCode_t err_code;
} ErrorData_t;


uint8_t Packet_BuildTelemetry(uint8_t *buffer, const TelemetryData_t *data);
uint8_t Packet_BuildError(uint8_t *buffer, const ErrorData_t *data);
uint8_t Packet_CalculateChecksum(const uint8_t *buffer, uint8_t len);

#endif // PACKET_H

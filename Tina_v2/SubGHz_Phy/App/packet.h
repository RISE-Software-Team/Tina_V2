/*
 * packet.h
 *
 *  Created on: Oct 7, 2025
 *      Author: krissal1234
 */

#ifndef PACKET_H
#define PACKET_H

#include <logger.h>
#include <stdint.h>


typedef enum {
    PACKET_TYPE_TELEMETRY = 0x01,
    PACKET_TYPE_LOG       = 0x02,
} PacketType_t;

typedef struct {
    uint8_t  header;
    uint32_t seq_num;
    uint8_t  packet_len;
    uint32_t timestamp;
    uint8_t  packet_type;
} HeaderPacket;

typedef struct {
    int16_t acc_x, acc_y, acc_z;
    int16_t gyro_x, gyro_y, gyro_z;
    uint16_t pressure;
    uint16_t altitude;
    uint8_t fsm_state;
} TelemetryPacket_t;

typedef struct {
	MessageCode_t code;
	uint8_t message_len;
	const char *message;
} LogPacket_t;


uint8_t packet_build_telemetry(uint8_t *buffer, TelemetryPacket_t data);
uint8_t packet_build_log(uint8_t *buffer, LogPacket_t data);

#endif // PACKET_H

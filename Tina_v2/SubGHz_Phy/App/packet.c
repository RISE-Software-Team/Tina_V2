/*
 * packet.c
 *
 *  Created on: Oct 7, 2025
 *      Author: krissal1234
 */

#include "packet.h"

#include "main.h"
#include "config.h"

#include <string.h>

#define PACKET_HEADER 0xAA

uint32_t seq_num = 0;

static void pack_16(uint8_t *buf, uint16_t value) {
    buf[0] = (value >> 8) & 0xFF;
    buf[1] = value & 0xFF;
}

static void pack_32(uint8_t *buf, uint32_t value) {
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8)  & 0xFF;
    buf[3] = value & 0xFF;
}

uint8_t packet_calculate_checksum(const uint8_t *buffer, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 1; i < len; ++i) {
        crc ^= buffer[i];
    }
    return crc;
}
uint8_t packet_build_header(uint8_t *buffer){
    uint8_t idx = 0;

    buffer[idx++] = PACKET_HEADER;
    pack_32(&buffer[idx], seq_num++); idx += 4;

    idx += 1; // length placeholder

    pack_32(&buffer[idx], HAL_GetTick()); idx += 4; //must change this to time of launch not time since boot

    return idx;

}
uint8_t packet_build_telemetry(uint8_t *buffer, TelemetryPacket_t data) {

	uint8_t idx = packet_build_header(buffer);

    buffer[idx++] = PACKET_TYPE_TELEMETRY;
    pack_16(&buffer[idx], data.acc_x); idx += 2;
    pack_16(&buffer[idx], data.acc_y); idx += 2;
    pack_16(&buffer[idx], data.acc_z); idx += 2;
    pack_16(&buffer[idx], data.gyro_x); idx += 2;
    pack_16(&buffer[idx], data.gyro_y); idx += 2;
    pack_16(&buffer[idx], data.gyro_z); idx += 2;
    pack_16(&buffer[idx], data.pressure); idx += 2;
    pack_16(&buffer[idx], data.altitude); idx += 2;
    buffer[idx++] = data.fsm_state;
    buffer[5] = idx + 1; //accounting for the checksum

    uint8_t crc = packet_calculate_checksum(buffer, idx);
    buffer[idx++] = crc;

    return idx;
}

uint8_t packet_build_log(uint8_t *buffer, LogPacket_t data)
{
    uint8_t idx = packet_build_header(buffer);

    buffer[idx++] = PACKET_TYPE_LOG;
    buffer[idx++] = (uint8_t)((int8_t)data.code);

    uint8_t msg_len = data.message_len;
    if (msg_len > MAX_LOG_MESSAGE_LEN) {
        msg_len = MAX_LOG_MESSAGE_LEN; //truncate if too big
    }
    buffer[idx++] = msg_len;

    if (msg_len != 0 && data.message != NULL) {
        memcpy(&buffer[idx], data.message, msg_len);
        idx += msg_len;
    }

    buffer[5] = idx + 1;

    uint8_t crc = packet_calculate_checksum(buffer, idx);
    buffer[idx++] = crc;

    return idx;
}

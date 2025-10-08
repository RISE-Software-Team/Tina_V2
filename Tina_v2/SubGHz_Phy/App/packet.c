/*
 * packet.c
 *
 *  Created on: Oct 7, 2025
 *      Author: krissal1234
 */

#include "packet.h"
#include "main.h"

#define PACKET_HEADER 0xAA

static uint16_t seq_num = 0;


static void Pack16(uint8_t *buf, uint16_t value) {
    buf[0] = (value >> 8) & 0xFF;
    buf[1] = value & 0xFF;
}

static void Pack32(uint8_t *buf, uint32_t value) {
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8)  & 0xFF;
    buf[3] = value & 0xFF;
}

uint8_t Packet_CalculateChecksum(const uint8_t *buffer, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 1; i < len; ++i) {
        crc ^= buffer[i];
    }
    return crc;
}
uint8_t Packet_BuildHeader(uint8_t *buffer){
    uint8_t idx = 0;
    buffer[idx++] = PACKET_HEADER;
    buffer[idx++] = (seq_num >> 8) & 0xFF;
    buffer[idx++] = seq_num & 0xFF;
    seq_num++;

    idx += 1; // length placeholder
    uint32_t ts = HAL_GetTick(); //must change this to time of launch not time since boot
    Pack32(&buffer[idx], ts); idx += 4;

    return idx;

}
uint8_t Packet_BuildTelemetry(uint8_t *buffer, const TelemetryData_t *data) {

	uint8_t idx = Packet_BuildHeader(buffer);

    buffer[idx++] = PACKET_TYPE_TELEMETRY;
    Pack16(&buffer[idx], data->acc_x); idx += 2;
    Pack16(&buffer[idx], data->acc_y); idx += 2;
    Pack16(&buffer[idx], data->acc_z); idx += 2;
    Pack16(&buffer[idx], data->gyro_x); idx += 2;
    Pack16(&buffer[idx], data->gyro_y); idx += 2;
    Pack16(&buffer[idx], data->gyro_z); idx += 2;
    Pack32(&buffer[idx], data->altitude); idx += 4;
    buffer[idx++] = data->event_flags;
    buffer[idx++] = data->sys_state;

    uint8_t crc = Packet_CalculateChecksum(buffer, idx);
    buffer[idx++] = crc;
    buffer[3] = idx;

    return idx;
}

uint8_t Packet_BuildError(uint8_t *buffer, const ErrorData_t *data) {

	uint8_t idx = Packet_BuildHeader(buffer);

    buffer[idx++] = PACKET_TYPE_ERROR;
    buffer[idx++] = data->severity;
    buffer[idx++] = data->err_code;

    uint8_t crc = Packet_CalculateChecksum(buffer, idx);
    buffer[idx++] = crc;
    buffer[3] = idx;

    return idx;
}

uint8_t Packet_BuildInfo(uint8_t *buffer, const InfoData_t *data) {

	uint8_t idx = Packet_BuildHeader(buffer);

    buffer[idx++] = PACKET_TYPE_INFO;

    buffer[idx++] = data->info_code;

    uint8_t crc = Packet_CalculateChecksum(buffer, idx);
    buffer[idx++] = crc;
    buffer[3] = idx;

    return idx;
}

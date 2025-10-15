/*
 * logger.c
 *
 *  Created on: Oct 13, 2025
 *      Author: krissal1234
 */

#include <logger.h>
#include "subghz_phy_app.h"
#include "packet.h"
#include <stdio.h>
#include "global_config.h"
#include <string.h>

void tlog(MessageCode_t code, const char *msg)
{
#if (DEBUG == 1)
    // Debug mode: send log with message
    LogPacket_t log_packet;
    log_packet.code = code;

    if (msg != NULL) {
        log_packet.message = msg;
        log_packet.message_len = (uint8_t)strnlen(msg, MAX_PACKET_LENGTH);
    } else {
        log_packet.message = NULL;
        log_packet.message_len = 0;
    }

    subghz_send_log_packet(&log_packet);

#else
    LogPacket_t log_packet;
    log_packet.code = code;
    log_packet.message = NULL;
    log_packet.message_len = 0;

    subghz_send_log_packet(&log_packet);
#endif

#if (ENABLE_SERIAL_LOG == 1)
    // Optional serial output
    const char *type_str = ((int8_t)code < 0) ? "ERROR" : "INFO";
    printf("[LOG] %s | Code: 0x%02X | %s\n",
           type_str,
           (uint8_t)((int8_t)code),
           msg ? msg : "");
#endif
}

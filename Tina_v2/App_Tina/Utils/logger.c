#include "logger.h"

#include "config.h"
#include "packet.h"
#include "subghz_phy_app.h"

#include <stdio.h>
#include <string.h>

void tlog(MessageCode_t code, const char *msg) {
	LogPacket_t packet;

	packet.code = code;
	packet.message = NULL;
	packet.message_len = 0;

#ifdef DEBUG
	packet.message = msg;
	packet.message_len = (uint8_t) strnlen(msg, MAX_LOG_MESSAGE_LEN);
#endif

	subghz_send_log_packet(packet);

#ifdef ENABLE_SERIAL_LOG
    printf("[LOG] %s | Code: 0x%02X | %s\n",
    	   (code > 0) ? "INFO" : "ERROR",
           (uint8_t)((int8_t)code),
           msg ? msg : "");
#endif
}

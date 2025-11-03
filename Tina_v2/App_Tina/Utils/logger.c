#include "logger.h"

#include "main.h"
#include "config.h"
#include "packet.h"
#include "subghz_phy_app.h"
#include "types.h"

#include <stdio.h>
#include <string.h>


#define NUM_MESSAGE_CODES 256 // (-128, 127)

static u32 last_log_time[NUM_MESSAGE_CODES] = {0};


static bool is_high_priority(MessageCode_t code)
{
	switch (code) {
	case INFO_DEBUG:
	case ERR_IMU_READ_ACCEL_FAIL:
	case ERR_IMU_READ_GYRO_FAIL:
	case ERR_BARO_READ_FAIL:
	case ERR_I2C_LINE_FAIL:
	case ERR_FSM_STATE_FAIL:
	case ERR_MISC_ERR:
		return false;
	default:
		return true;
	}
}

void tlog(MessageCode_t code, const char *msg)
{
	LogPacket_t packet;

	if (!is_high_priority(code)
			&& (last_log_time[code + 128] != 0)
			&& (HAL_GetTick() - last_log_time[code + 128] < LOG_INTERVAL_MS))
		return;

	last_log_time[code + 128] = HAL_GetTick();

	packet.code = code;
	packet.message = NULL;
	packet.message_len = 0;

#ifdef TLOG_DEBUG
	packet.message = msg;
	packet.message_len = (u8)strnlen(msg, MAX_LOG_MESSAGE_LEN);
#endif

	radio_send_log_packet(packet);

#ifdef ENABLE_SERIAL_LOG
    printf("[LOG] %s | Code: 0x%02X | %s\n",
    	   (code > 0) ? "INFO" : "ERROR",
           (u8)((s8)code),
           msg ? msg : "");
#endif
}

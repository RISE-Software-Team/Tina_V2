#include "telemetry.h"

#include "flight_fsm.h"
#include "main.h"
#include "packet.h"
#include "subghz_phy_app.h"
#include "types.h"

void telemetry_send(FlightFSM_t *fsm)
{
	static uint32_t last_telemetry_time = 0;
	TelemetryPacket_t telemetry;

	if (HAL_GetTick() - last_telemetry_time < TELEMETRY_INTERVAL_MS)
		return;

	if (fsm->status.imu_ok) {
		telemetry.acc_x = (int16_t)(fsm->sensor_data.acc_x * 100);
		telemetry.acc_y = (int16_t)(fsm->sensor_data.acc_y * 100);
		telemetry.acc_z = (int16_t)(fsm->sensor_data.acc_z * 100);
		telemetry.gyro_x = (int16_t)fsm->sensor_data.gyro_x;
		telemetry.gyro_y = (int16_t)fsm->sensor_data.gyro_y;
		telemetry.gyro_z = (int16_t)fsm->sensor_data.gyro_z;
	} else {
		telemetry.acc_x = -1;
		telemetry.acc_y = -1;
		telemetry.acc_z = -1;
		telemetry.gyro_x = -1;
		telemetry.gyro_y = -1;
		telemetry.gyro_z = -1;
	}

	if (fsm->status.baro_ok) {
		telemetry.pressure = (int16_t)(fsm->sensor_data.pres / 100);
		telemetry.altitude = (int16_t)fsm->hist.avg_alt;
	} else {
		telemetry.pressure = -1;
		telemetry.altitude = -1;
	}

	telemetry.fsm_state = fsm->state;

	radio_send_telemetry_packet(telemetry);

	last_telemetry_time = HAL_GetTick();
}

#include "telemetry.h"

#include "main.h"
#include "packet.h"
#include "subghz_phy_app.h"
#include "types.h"

void telemetry_send(SensorData_t sensor_data, float altitude, uint8_t fsm_state)
{
	static uint32_t last_telemetry_time = 0;
	TelemetryPacket_t telemetry;

	if (HAL_GetTick() - last_telemetry_time < TELEMETRY_INTERVAL_MS)
		return;

	telemetry.acc_x = (int16_t)sensor_data.acc_x;
	telemetry.acc_y = (int16_t)sensor_data.acc_y;
	telemetry.acc_z = (int16_t)sensor_data.acc_z;
	telemetry.gyro_x = (int16_t)sensor_data.gyro_x;
	telemetry.gyro_y = (int16_t)sensor_data.gyro_y;
	telemetry.gyro_z = (int16_t)sensor_data.gyro_z;

	if (sensor_data.pres > 0.0) {
		telemetry.pressure = (int16_t)(sensor_data.pres / 100);
		telemetry.altitude = (int16_t)altitude;
	} else {
		telemetry.pressure = (int16_t)sensor_data.pres;
		telemetry.altitude = -1;
	}

	telemetry.fsm_state = fsm_state;

	radio_send_telemetry_packet(telemetry);

	last_telemetry_time = HAL_GetTick();
}

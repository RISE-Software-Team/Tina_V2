/*
 * sensors.c
 *
 *  Created on: Oct 15, 2025
 *      Author: iancu
 */

#include "sensor.h"

#include "bme280_api.h"
#include "bno055_api.h"
#include "config.h"
#include "logger.h"
#include "main.h"

#include <stdint.h>
#include <stdbool.h>

int8_t sensors_read_all(SensorData_t *data)
{
	BNO055_AccelData_t bno055_accel;
	BNO055_GyroData_t bno055_gyro;
	float pressure;
	bool baro_failed = false;
	bool imu_failed = false;

	if (data == NULL)
		return -1;

	if (BME280_ReadPressure(&pressure)) {
		data->pressure = -1;

		baro_failed = true;
		tlog(ERR_BARO_READ_PRESSURE_FAIL, "Baro pressure read failed");
	} else {
		data->pressure = pressure;
	}

	if ((rc = BNO055_ReadAccel(&bno055_accel))) {
		data->acc_x = -1;
		data->acc_y = -1;
		data->acc_z = -1;

		imu_failed = true;
		tlog(ERR_IMU_READ_ACCEL_FAIL, "IMU accel read failed");
	} else {
		data->acc_x = bno055_accel.x;
		data->acc_y = bno055_accel.y;
		data->acc_z = bno055_accel.z;
	}

	if ((rc = BNO055_ReadGyro(&bno055_gyro))) {
		data->gyro_x = -1;
		data->gyro_y = -1;
		data->gyro_z = -1;

		imu_failed = true;
		tlog(ERR_IMU_READ_GYRO_FAIL, "IMU gyro read failed");
	} else {
		data->gyro_x = bno055_gyro.x;
		data->gyro_y = bno055_gyro.y;
		data->gyro_z = bno055_gyro.z;
	}

	return (-1 * baro_failed) + (-2 * imu_failed);
}

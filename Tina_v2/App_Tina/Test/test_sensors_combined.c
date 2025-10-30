#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "../Sensors/BME280/bme280_mock_driver.h"
#include "../Sensors/BME280/bme280_api.h"
#include "../Sensors/BNO055/bno055_mock_driver.h"
#include "../Sensors/BNO055/bno055_api.h"

int main(void) {
    printf("BME280 + BNO055 ZMQ compact test starting...\n");

    // Register mock drivers
    BME280_RegisterDriver(&bme280_default_driver);
    BNO055_RegisterDriver(&bno055_default_driver);

    // Initialize BME280
    if (BME280_Init() != 0) {
        fprintf(stderr, "BME280_Init failed: publisher may be down\n");
        return 1;
    }

    // Initialize BNO055
    if (BNO055_Init() != 0) {
        fprintf(stderr, "BNO055_Init failed: publisher may be down\n");
        return 1;
    }

    printf("Both BME280 and BNO055 initialized successfully\n");

    float temp, press, hum;
    BNO055_AccelData_t accel;
    BNO055_GyroData_t gyro;
    BNO055_EulerData_t euler;
    double accel_mag;

    // Read values continuously
    while (1) {
        // Timestamp
        unsigned long ts = (unsigned long)time(NULL);

        // BME280
        if (BME280_ReadAll(&temp, &press, &hum) != 0) {
            printf("[%lu] BME280 read failed\n", ts);
        }

        // BNO055
        if (BNO055_ReadAccel(&accel) != 0 ||
            BNO055_ReadGyro(&gyro) != 0 ||
            BNO055_ReadEuler(&euler) != 0) {
            printf("[%lu] BNO055 read failed\n", ts);
        }

        // Compute acceleration magnitude
        accel_mag = sqrt(accel.x*accel.x + accel.y*accel.y + accel.z*accel.z);

        // Compact single-line summary
        printf("[%lu] Temp: %.2f°C | Press: %.2f hPa | Hum: %.2f%% | "
               "Accel: X=%.2f Y=%.2f Z=%.2f (Mag=%.2f) | "
               "Gyro: X=%.2f Y=%.2f Z=%.2f | "
               "Euler: H=%.2f R=%.2f P=%.2f\n",
               ts,
               temp, press, hum,
               accel.x, accel.y, accel.z, accel_mag,
               gyro.x, gyro.y, gyro.z,
               euler.heading, euler.roll, euler.pitch
        );

        usleep(500000*2); // 500ms delay
    }

    return 0;
}

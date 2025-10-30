#include <stdio.h>
#include <unistd.h>
#include "../Sensors/BNO055/bno055_mock_driver.h"
#include "../Sensors/BNO055/bno055_api.h"

int main(void) {
    printf("BNO055 ZMQ test starting...\n");

    // Register the mock driver
    BNO055_RegisterDriver(&bno055_default_driver);

    // Initialize BNO055 (will fail if publisher is down)
    if (BNO055_Init() != 0) {
        fprintf(stderr, "BNO055_Init failed: publisher may be down\n");
        return 1;
    }

    printf("BNO055 initialized successfully\n");

    BNO055_AccelData_t accel;
    BNO055_GyroData_t gyro;
    BNO055_EulerData_t euler;

    // Read values continuously
    while (1) {
        // Read accelerometer
        if (BNO055_ReadAccel(&accel) == 0) {
            printf("Accel: X=%.2f m/s² | Y=%.2f m/s² | Z=%.2f m/s²\n",
                   accel.x, accel.y, accel.z);
        } else {
            printf("Failed to read accelerometer\n");
        }

        // Read gyroscope
        if (BNO055_ReadGyro(&gyro) == 0) {
            printf("Gyro:  X=%.2f °/s | Y=%.2f °/s | Z=%.2f °/s\n",
                   gyro.x, gyro.y, gyro.z);
        } else {
            printf("Failed to read gyroscope\n");
        }

        // Read euler angles
        if (BNO055_ReadEuler(&euler) == 0) {
            printf("Euler: Heading=%.2f° | Roll=%.2f° | Pitch=%.2f°\n",
                   euler.heading, euler.roll, euler.pitch);
        } else {
            printf("Failed to read euler angles\n");
        }

        printf("---\n");
        sleep(1);
    }

    return 0;
}
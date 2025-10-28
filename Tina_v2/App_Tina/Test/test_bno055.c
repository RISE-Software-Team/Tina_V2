#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../Sensors/BNO055/bno055_api.c"


int main(void) {

    printf("BNO055 host test starting...\n");

    /* Register the mock driver explicitly */
    BNO055_RegisterDriver(NULL);

    if (BNO055_Init() != 0) {
        printf("BNO055_Init failed\n");
        return 1;
    }

    BNO055_AccelData_t accel;
    BNO055_GyroData_t gyro;
    BNO055_EulerData_t euler;

    for (int i = 0; i < 5; i++) {
        printf("\nReading %d:\n", i + 1);
        
        if (BNO055_ReadAccel(&accel) == 0) {
            printf("Accel: x=%.3f m/s^2, y=%.3f, z=%.3f\n", accel.x, accel.y, accel.z);
        } else {
            printf("BNO055_ReadAccel failed\n");
            return -1;
        }

        if (BNO055_ReadGyro(&gyro) == 0) {
            printf("Gyro: x=%.3f deg/s, y=%.3f, z=%.3f\n", gyro.x, gyro.y, gyro.z);
        } else {
            printf("BNO055_ReadGyro failed\n");
            return -1;
        }

        if (BNO055_ReadEuler(&euler) == 0) {
            printf("Euler: heading=%.3f deg, roll=%.3f deg, pitch=%.3f deg\n", euler.heading, euler.roll, euler.pitch);
        } else {
            printf("BNO055_ReadEuler failed\n");
            return -1;
        }

        sleep(1); // Sleep 1 second between readings
    }


    return 0;
}
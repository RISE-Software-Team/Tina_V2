#include <stdio.h>
#include <stdlib.h>
#include "host_includes.h"
#include "main.h"
#include "../Sensors/BME280/bme280_api.h"

/* The mock driver is defined in the parent directory */
extern const BME280_Driver_t mock_driver;

int main(void) {
    printf("BME280 host test starting...\n");

    /* Register the mock driver explicitly */
    BME280_RegisterDriver(&mock_driver);

    if (BME280_Init() != BME280_OK) {
        printf("BME280_Init failed\n");
        return 1;
    }

    float t=0, p=0, h=0;
    if (BME280_ReadAll(&t, &p, &h) == 0) {
        printf("Temperature: %.2f C, Pressure: %.2f hPa, Humidity: %.2f %%\n", t, p, h);
    } else {
        printf("BME280_ReadAll failed\n");
        return 2;
    }

    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include "../Sensors/BME280/bme280_mock_driver.h"
#include "../Sensors/BME280/bme280_api.h"

int main(void) {
    printf("BME280 ZMQ test starting...\n");

    // Register the mock driver
    BME280_RegisterDriver(&bme280_default_driver);

    // Initialize BME280 (will fail if publisher is down)
    if (BME280_Init() != 0) {
        fprintf(stderr, "BME280_Init failed: publisher may be down\n");
        return 1;
    }

    printf("BME280 initialized successfully\n");

    float temp, press, hum;

    // Read values continuously
    while (1) {
        if (BME280_ReadAll(&temp, &press, &hum) == 0) {
            printf("Temp: %.2f °C | Pressure: %.2f hPa | Humidity: %.2f %%\n",
                   temp, press, hum);
        } else {
            printf("Failed to read BME280 values\n");
        }
        sleep(1);
    }

    return 0;
}
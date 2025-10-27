/* default_driver_stub.c
 * Provide a stub 'default_driver' for linking when building on host.
 * The real default_driver is part of the firmware; for host tests we
 * provide a minimal implementation that returns errors if used.
 */


#include "host_includes.h"
#include "../Sensors/BME280/bme280_api.h"

const BME280_Driver_t default_driver = {
    .init = NULL,
    .read_temperature = NULL,
    .read_pressure = NULL,
    .read_humidity = NULL,
    .read_all = NULL,
};

void Error_Handler(void) {
    /* For host test, just print and exit */
    fprintf(stderr, "Error_Handler called\n");
    fflush(stderr);
    abort();
}

#include "bme280_api.h"
#include "main.h"

/* -------------------------------------------------------------------------- */
/* Mock-level operations                                                  */
/* -------------------------------------------------------------------------- */
static s32 mock_init(void);
static int mock_read_all(float *temp, float *press, float *hum);
static int mock_read_temperature(float *temp);
static int mock_read_pressure(float *press);
static int mock_read_humidity(float *hum);

/* -------------------------------------------------------------------------- */
/* Mock hardware driver instance                                           */
/* -------------------------------------------------------------------------- */
const BME280_Driver_t mock_driver = {
    .init = mock_init,
    .read_temperature = mock_read_temperature,
    .read_pressure = mock_read_pressure,
    .read_humidity = mock_read_humidity,
    .read_all = mock_read_all,
};

static s32 mock_init(void) {
    /* Mock initialization logic */
    return BME280_OK;
}

static int mock_read_temperature(float *temp) {
    *temp = 25.0; // Mock temperature value
    return 0;
}

static int mock_read_pressure(float *press) {
    *press = 1013.25; // Mock pressure value
    return 0;
}

static int mock_read_humidity(float *hum) {
    *hum = 40.0; // Mock humidity value
    return 0;
}

static int mock_read_all(float *temp, float *press, float *hum) {
    mock_read_temperature(temp);
    mock_read_pressure(press);
    mock_read_humidity(hum);
    return 0;
}

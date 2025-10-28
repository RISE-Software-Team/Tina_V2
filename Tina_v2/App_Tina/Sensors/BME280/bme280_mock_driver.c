#include <stdlib.h>

#include "bme280_api.h"
#include "main.h"

/* -------------------------------------------------------------------------- */
/* Mock-level operations                                                      */
/* -------------------------------------------------------------------------- */
static s32 mock_init(void);
static int mock_read_all(float *temp, float *press, float *hum);
static int mock_read_temperature(float *temp);
static int mock_read_pressure(float *press);
static int mock_read_humidity(float *hum);

/* -------------------------------------------------------------------------- */
/* Base Values                                                                */
/* -------------------------------------------------------------------------- */
static float BASE_TEMP  = 25.0f;
static float BASE_PRESS = 1000.0f;
static float BASE_HUM   = 50.0f;

static float add_random_noise(float base, float range)
{
    // range = ±range (so range=1 gives random change between -1 and +1)
    float noise = ((float)(rand() % 2001) / 1000.0f - 1.0f) * range;
    return base + noise;
}

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
    return 0;
}

static int mock_read_temperature(float *temp) {
    *temp = add_random_noise(BASE_TEMP, 1.0f);
    return 0;
}

static int mock_read_pressure(float *press) {
    *press = add_random_noise(BASE_PRESS, 1.0f);
    return 0;
}

static int mock_read_humidity(float *hum) {
    *hum = add_random_noise(BASE_HUM, 1.0f);
    return 0;
}

static int mock_read_all(float *temp, float *press, float *hum) {
    mock_read_temperature(temp);
    mock_read_pressure(press);
    mock_read_humidity(hum);
    return 0;
}

#include "bme280_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <time.h>

/* Latest received values from ZMQ */
static float latest_temp  = 25.0f;
static float latest_press = 1000.0f;
static float latest_hum   = 50.0f;


/* ZMQ handles */
static void *context    = NULL;
static void *subscriber = NULL;

/* -------------------------------------------------------------------------- */
/* Utility: Add small random noise when no new data is available              */
/* -------------------------------------------------------------------------- */
static float add_random_noise(float base, float range)
{
    float noise = ((float)(rand() % 2001) / 1000.0f - 1.0f) * range;
    return base + noise;
}

/* -------------------------------------------------------------------------- */
/* Receive latest JSON message from ZMQ publisher                             */
/* -------------------------------------------------------------------------- */
static void receive_update()
{
    zmq_msg_t topic, payload;
    zmq_msg_init(&topic);
    zmq_msg_init(&payload);

    // Try to receive both topic and message non-blocking
    int rc1 = zmq_msg_recv(&topic, subscriber, ZMQ_DONTWAIT);
    if (rc1 < 0) {
        zmq_msg_close(&topic);
        zmq_msg_close(&payload);
        return;
    }

    int rc2 = zmq_msg_recv(&payload, subscriber, ZMQ_DONTWAIT);
    if (rc2 < 0) {
        zmq_msg_close(&topic);
        zmq_msg_close(&payload);
        return;
    }

    size_t size = zmq_msg_size(&payload);
    char *data = (char *)malloc(size + 1);
    memcpy(data, zmq_msg_data(&payload), size);
    data[size] = '\0';

    // Parse JSON manually (simple and dependency-free)
    float t, p, h;
    if (sscanf(data, "{\"temperature\":%f,\"pressure\":%f,\"humidity\":%f", &t, &p, &h) == 3) {
        latest_temp  = t;
        latest_press = p;
        latest_hum   = h;
    }

    free(data);
    zmq_msg_close(&topic);
    zmq_msg_close(&payload);
}

/* -------------------------------------------------------------------------- */
/* Mock-level operations                                                      */
/* -------------------------------------------------------------------------- */
static int32_t mock_init(void);
static int mock_read_all(float *temp, float *press, float *hum);
static int mock_read_temperature(float *temp);
static int mock_read_pressure(float *press);
static int mock_read_humidity(float *hum);

/* -------------------------------------------------------------------------- */
/* Default hardware driver instance                                           */
/* -------------------------------------------------------------------------- */
const BME280_Driver_t bme280_default_driver = {
    .init = mock_init,
    .read_temperature = mock_read_temperature,
    .read_pressure = mock_read_pressure,
    .read_humidity = mock_read_humidity,
    .read_all = mock_read_all,
};

/* -------------------------------------------------------------------------- */
/* Initialize BME280 mock (connect to ZMQ publisher)                          */
/* -------------------------------------------------------------------------- */
static int32_t mock_init(void)
{
    srand(time(NULL));
    context = zmq_ctx_new();
    subscriber = zmq_socket(context, ZMQ_SUB);

    // Connect to simulator
    int rc = zmq_connect(subscriber, "tcp://localhost:5555");
    if (rc != 0) {
        fprintf(stderr, "BME280 mock: Failed to connect to ZMQ\n");
        return -1;
    }

    // Subscribe only to "BME280"
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "BME280", 6);

    // Set receive timeout (in milliseconds)
    int timeout = 2000; // 2 seconds
    zmq_setsockopt(subscriber, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));

    // Try receiving a message to confirm publisher is up
    zmq_msg_t topic, payload;
    zmq_msg_init(&topic);
    zmq_msg_init(&payload);

    int recv1 = zmq_msg_recv(&topic, subscriber, 0);
    int recv2 = zmq_msg_recv(&payload, subscriber, 0);

    zmq_msg_close(&topic);
    zmq_msg_close(&payload);

    if (recv1 < 0 || recv2 < 0) {
        fprintf(stderr, "BME280 mock: No publisher detected (timeout %d ms)\n", timeout);
        return -1;
    }

    printf("BME280 mock: Publisher detected and connected\n");
    return 0;
}


/* -------------------------------------------------------------------------- */
/* Combined read of all BME280 values                                         */
/* -------------------------------------------------------------------------- */
static int mock_read_all(float *temp, float *press, float *hum)
{
    receive_update();

    // Use latest or fallback to noise
    if (temp)  *temp  = add_random_noise(latest_temp, 0.1f);
    if (press) *press = add_random_noise(latest_press, 0.2f);
    if (hum)   *hum   = add_random_noise(latest_hum, 0.5f);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Individual read wrappers                                                   */
/* -------------------------------------------------------------------------- */
static int mock_read_temperature(float *temp)
{
    receive_update();
    *temp = add_random_noise(latest_temp, 0.1f);
    return 0;
}

static int mock_read_pressure(float *press)
{
    receive_update();
    *press = add_random_noise(latest_press, 0.2f);
    return 0;
}

static int mock_read_humidity(float *hum)
{
    receive_update();
    *hum = add_random_noise(latest_hum, 0.5f);
    return 0;
}

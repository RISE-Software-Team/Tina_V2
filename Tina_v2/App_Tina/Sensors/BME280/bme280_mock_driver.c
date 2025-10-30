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
/* Helper to parse a float from JSON key                                       */
/* -------------------------------------------------------------------------- */
static int parse_float(const char *data, const char *key, float *value) {
    char *pos = strstr(data, key);
    if (!pos) return -1;
    pos += strlen(key);  // move past the key
    while (*pos && (*pos < '0' || *pos > '9') && *pos != '-' && *pos != '.') pos++; // skip to number
    if (*pos == '\0') return -1;
    *value = strtof(pos, NULL);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Receive latest JSON message from ZMQ publisher                             */
/* -------------------------------------------------------------------------- */
static void receive_update() {
    zmq_msg_t topic, payload;
    zmq_msg_init(&topic);
    zmq_msg_init(&payload);

    // Try to receive both topic and message non-blocking
    int rc1 = zmq_msg_recv(&topic, subscriber, ZMQ_DONTWAIT);
    if (rc1 < 0) goto cleanup;
    int rc2 = zmq_msg_recv(&payload, subscriber, ZMQ_DONTWAIT);
    if (rc2 < 0) goto cleanup;

    size_t size = zmq_msg_size(&payload);
    char *data = (char *)malloc(size + 1);
    memcpy(data, zmq_msg_data(&payload), size);
    data[size] = '\0';

    // Parse each value individually
    float t, p, h;
    if (parse_float(data, "\"temperature\":", &t) == 0) latest_temp = t;
    if (parse_float(data, "\"pressure\":", &p) == 0) latest_press = p;
    if (parse_float(data, "\"humidity\":", &h) == 0) latest_hum = h;

    free(data);

cleanup:
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
static int32_t mock_init(void) {
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

    // Set receive timeout (ms)
    int timeout = 20000; // 20s
    zmq_setsockopt(subscriber, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));

    // Confirm publisher is up by trying to receive first message
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
static int mock_read_all(float *temp, float *press, float *hum) {
    receive_update();
    if (temp)  *temp  = latest_temp;
    if (press) *press = latest_press;
    if (hum)   *hum   = latest_hum;
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Individual read wrappers                                                   */
/* -------------------------------------------------------------------------- */
static int mock_read_temperature(float *temp) {
    receive_update();
    if (temp) *temp = latest_temp;
    return 0;
}

static int mock_read_pressure(float *press) {
    receive_update();
    if (press) *press = latest_press;
    return 0;
}

static int mock_read_humidity(float *hum) {
    receive_update();
    if (hum) *hum = latest_hum;
    return 0;
}

#include "bno055_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <time.h>
#include <math.h>

/* Latest received values from ZMQ */
static float latest_accel[3] = {0.0f, 0.0f, -9.81f};
static float latest_gyro[3]  = {0.0f, 0.0f, 0.0f};
static float latest_euler[3] = {0.0f, 0.0f, 0.0f};

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

/* Helper to parse a float array from JSON key */
static void parse_array(const char *data, const char *key, float *arr, int n) {
    char *pos = strstr(data, key);
    if (!pos) return;
    pos = strchr(pos, '[');
    if (!pos) return;
    pos++; // skip '['
    for (int i = 0; i < n; i++) {
        while (*pos && (*pos < '0' || *pos > '9') && *pos != '-' && *pos != '.') pos++;
        if (*pos == '\0') break;
        arr[i] = strtof(pos, &pos);
    }
}

/* -------------------------------------------------------------------------- */
/* Receive latest JSON message from ZMQ publisher                             */
/* -------------------------------------------------------------------------- */
static void receive_update() {
    zmq_msg_t topic, payload;
    zmq_msg_init(&topic);
    zmq_msg_init(&payload);

    int rc1 = zmq_msg_recv(&topic, subscriber, ZMQ_DONTWAIT);
    if (rc1 < 0) goto cleanup;
    int rc2 = zmq_msg_recv(&payload, subscriber, ZMQ_DONTWAIT);
    if (rc2 < 0) goto cleanup;

    size_t size = zmq_msg_size(&payload);
    char *data = (char *)malloc(size + 1);
    memcpy(data, zmq_msg_data(&payload), size);
    data[size] = '\0';

    // Parse arrays and euler angles
    parse_array(data, "\"accel\":", latest_accel, 3);
    parse_array(data, "\"gyro\":", latest_gyro, 3);
    parse_float(data, "\"heading\":", &latest_euler[0]);
    parse_float(data, "\"roll\":", &latest_euler[1]);
    parse_float(data, "\"pitch\":", &latest_euler[2]);

    free(data);

cleanup:
    zmq_msg_close(&topic);
    zmq_msg_close(&payload);
}

/* -------------------------------------------------------------------------- */
/* Mock-level operations                                                      */
/* -------------------------------------------------------------------------- */
static int8_t mock_init(void);
static int mock_read_accel(BNO055_AccelData_t *accel);
static int mock_read_gyro(BNO055_GyroData_t *gyro);
static int mock_read_euler(BNO055_EulerData_t *euler);

/* -------------------------------------------------------------------------- */
/* Default hardware driver instance                                           */
/* -------------------------------------------------------------------------- */
const BNO055_Driver_t bno055_default_driver = {
    .init = mock_init,
    .read_accel = mock_read_accel,
    .read_gyro = mock_read_gyro,
    .read_euler = mock_read_euler
};

/* -------------------------------------------------------------------------- */
/* Initialize BNO055 mock (connect to ZMQ publisher)                          */
/* -------------------------------------------------------------------------- */
static int8_t mock_init(void) {
    srand(time(NULL));
    context = zmq_ctx_new();
    subscriber = zmq_socket(context, ZMQ_SUB);

    int rc = zmq_connect(subscriber, "tcp://localhost:5555");
    if (rc != 0) {
        fprintf(stderr, "BNO055 mock: Failed to connect to ZMQ\n");
        return -1;
    }

    // Subscribe only to "BNO055"
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "BNO055", 6);

    int timeout = 20000; // 20s
    zmq_setsockopt(subscriber, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));

    // Confirm publisher is up
    zmq_msg_t topic, payload;
    zmq_msg_init(&topic);
    zmq_msg_init(&payload);
    int recv1 = zmq_msg_recv(&topic, subscriber, 0);
    int recv2 = zmq_msg_recv(&payload, subscriber, 0);
    zmq_msg_close(&topic);
    zmq_msg_close(&payload);

    if (recv1 < 0 || recv2 < 0) {
        fprintf(stderr, "BNO055 mock: No publisher detected (timeout %d ms)\n", timeout);
        return -1;
    }

    printf("BNO055 mock: Publisher detected and connected\n");
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Read wrappers                                                              */
/* -------------------------------------------------------------------------- */
static int mock_read_accel(BNO055_AccelData_t *accel) {
    receive_update();
    if (!accel) return -1;
    accel->x = latest_accel[0];
    accel->y = latest_accel[1];
    accel->z = latest_accel[2];
    return 0;
}

static int mock_read_gyro(BNO055_GyroData_t *gyro) {
    receive_update();
    if (!gyro) return -1;
    gyro->x = latest_gyro[0];
    gyro->y = latest_gyro[1];
    gyro->z = latest_gyro[2];
    return 0;
}

static int mock_read_euler(BNO055_EulerData_t *euler) {
    receive_update();
    if (!euler) return -1;
    euler->heading = latest_euler[0];
    euler->roll    = latest_euler[1];
    euler->pitch   = latest_euler[2];
    return 0;
}

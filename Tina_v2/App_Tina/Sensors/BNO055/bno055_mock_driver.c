#include "bno055_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <time.h>

/* Latest received values from ZMQ */
static float latest_accel_x = 0.0f;
static float latest_accel_y = 0.0f;
static float latest_accel_z = -9.81f;
static float latest_gyro_x  = 0.0f;
static float latest_gyro_y  = 0.0f;
static float latest_gyro_z  = 0.0f;
static float latest_heading = 0.0f;
static float latest_roll    = 0.0f;
static float latest_pitch   = 0.0f;

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
    // Format: {"accel":[x,y,z],"gyro":[x,y,z],"euler":{"heading":h,"roll":r,"pitch":p},"timestamp":t}
    float ax, ay, az, gx, gy, gz, h, r, p;
    if (sscanf(data, "{\"accel\":[%f,%f,%f],\"gyro\":[%f,%f,%f],\"euler\":{\"heading\":%f,\"roll\":%f,\"pitch\":%f",
               &ax, &ay, &az, &gx, &gy, &gz, &h, &r, &p) == 9) {
        latest_accel_x = ax;
        latest_accel_y = ay;
        latest_accel_z = az;
        latest_gyro_x  = gx;
        latest_gyro_y  = gy;
        latest_gyro_z  = gz;
        latest_heading = h;
        latest_roll    = r;
        latest_pitch   = p;
    }

    free(data);
    zmq_msg_close(&topic);
    zmq_msg_close(&payload);
}

/* -------------------------------------------------------------------------- */
/* Mock operations                                                            */
/* -------------------------------------------------------------------------- */
static int8_t mock_init(void);
static int mock_read_accel(BNO055_AccelData_t *accel);
static int mock_read_gyro(BNO055_GyroData_t *gyro);
static int mock_read_euler(BNO055_EulerData_t *euler);

/* -------------------------------------------------------------------------- */
/* Default mock driver instance                                               */
/* -------------------------------------------------------------------------- */
const BNO055_Driver_t bno055_default_driver = {
    .init = mock_init,
    .read_accel = mock_read_accel,
    .read_gyro  = mock_read_gyro,
    .read_euler = mock_read_euler,
};

/* -------------------------------------------------------------------------- */
/* Initialize BNO055 mock (connect to ZMQ publisher)                          */
/* -------------------------------------------------------------------------- */
static int8_t mock_init(void)
{
    srand(time(NULL));
    context = zmq_ctx_new();
    subscriber = zmq_socket(context, ZMQ_SUB);

    // Connect to simulator
    int rc = zmq_connect(subscriber, "tcp://localhost:5555");
    if (rc != 0) {
        fprintf(stderr, "BNO055 mock: Failed to connect to ZMQ\n");
        return -1;
    }

    // Subscribe only to "BNO055"
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "BNO055", 6);

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
        fprintf(stderr, "BNO055 mock: No publisher detected (timeout %d ms)\n", timeout);
        return -1;
    }

    printf("BNO055 mock: Publisher detected and connected\n");
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Sensor Read Implementations                                                */
/* -------------------------------------------------------------------------- */
static int mock_read_accel(BNO055_AccelData_t *accel)
{
    receive_update();

    // Use latest or fallback to noise
    accel->x = add_random_noise(latest_accel_x, 0.01f);
    accel->y = add_random_noise(latest_accel_y, 0.01f);
    accel->z = add_random_noise(latest_accel_z, 0.01f);

    return 0;
}

static int mock_read_gyro(BNO055_GyroData_t *gyro)
{
    receive_update();

    gyro->x = add_random_noise(latest_gyro_x, 0.5f);
    gyro->y = add_random_noise(latest_gyro_y, 0.5f);
    gyro->z = add_random_noise(latest_gyro_z, 0.5f);

    return 0;
}

static int mock_read_euler(BNO055_EulerData_t *euler)
{
    receive_update();

    euler->heading = add_random_noise(latest_heading, 0.1f);
    euler->roll    = add_random_noise(latest_roll, 0.1f);
    euler->pitch   = add_random_noise(latest_pitch, 0.1f);

    return 0;
}
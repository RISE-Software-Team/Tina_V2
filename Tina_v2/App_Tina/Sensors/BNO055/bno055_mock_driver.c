#include "bno055_api.h"

/* -------------------------------------------------------------------------- */
/* Base Values                                                                */
/* -------------------------------------------------------------------------- */
static float BASE_ACCL_X  = 0.05f;
static float BASE_ACCL_Y  = 0.03f;
static float BASE_ACCL_Z  = -9.81;
static float BASE_GYRO_X  = 0.0f;
static float BASE_GYRO_Y  = 0.0f;
static float BASE_GYRO_Z  = 0.0f;
static float BASE_EULER_H = 0.0f;
static float BASE_EULER_R = 0.0f;
static float BASE_EULER_P = 0.0f;

static float add_random_noise(float base, float range)
{
    // range = ±range (so range=1 gives random change between -1 and +1)
    float noise = ((float)(rand() % 2001) / 1000.0f - 1.0f) * range;
    return base + noise;
}


/* -------------------------------------------------------------------------- */
/* Mock operations                                                  */
/* -------------------------------------------------------------------------- */
static int8_t mock_init(void);
static int mock_read_accel(BNO055_AccelData_t *accel);
static int mock_read_gyro(BNO055_GyroData_t *gyro);
static int mock_read_euler(BNO055_EulerData_t *euler);

/* -------------------------------------------------------------------------- */
/* Default mock driver instance                                           */
/* -------------------------------------------------------------------------- */
const BNO055_Driver_t bno055_default_driver = {
    .init = mock_init,
    .read_accel = mock_read_accel,
    .read_gyro  = mock_read_gyro,
    .read_euler = mock_read_euler,
};

/* -------------------------------------------------------------------------- */
/* Mock Initialize BNO055 hardware                                           */
/* -------------------------------------------------------------------------- */
static int8_t mock_init(void) { return 0; }

/* -------------------------------------------------------------------------- */
/* Sensor Read Implementations                                                */
/* -------------------------------------------------------------------------- */
static int mock_read_accel(BNO055_AccelData_t *accel)
{
    accel->x = add_random_noise(BASE_ACCL_X, 0.01f);
    accel->y = add_random_noise(BASE_ACCL_Y, 0.01f);
    accel->z = add_random_noise(BASE_ACCL_Z, 0.5f);

    return 0;
}

static int mock_read_gyro(BNO055_GyroData_t *gyro)
{
    gyro->x = add_random_noise(BASE_GYRO_X, 0.01f);
    gyro->y = add_random_noise(BASE_GYRO_Y, 0.01f);
    gyro->z = add_random_noise(BASE_GYRO_Z, 0.01f);

    return 0;
}

static int mock_read_euler(BNO055_EulerData_t *euler)
{

    euler->heading = add_random_noise(BASE_EULER_H, 0.01f) / 16.0f;
    euler->roll    = add_random_noise(BASE_EULER_R, 0.01f) / 16.0f;
    euler->pitch   = add_random_noise(BASE_EULER_P, 0.01f) / 16.0f;

    return 0;
}
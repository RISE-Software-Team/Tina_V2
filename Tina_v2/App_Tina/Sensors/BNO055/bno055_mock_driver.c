#include "bno055_api.h"
#include "main.h"

static struct bno055_t bno055;

/* -------------------------------------------------------------------------- */
/* Mock-level operations                                                      */
/* -------------------------------------------------------------------------- */
static s8 mock_init(void);
static int mock_read_accel(BNO055_AccelData_t *accel);
static int mock_read_gyro(BNO055_GyroData_t *gyro);
static int mock_read_euler(BNO055_EulerData_t *euler);

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
/* Default hardware driver instance                                           */
/* -------------------------------------------------------------------------- */
const BNO055_Driver_t bno055_default_driver = {
    .init = mock_init,
    .read_accel = mock_read_accel,
    .read_gyro  = mock_read_gyro,
    .read_euler = mock_read_euler,
};

/* -------------------------------------------------------------------------- */
/* Mock Initialization                                                        */
/* -------------------------------------------------------------------------- */
static s8 mock_init(void)
{
    return BNO055_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Sensor Read Implementations                                                */
/* -------------------------------------------------------------------------- */
static int mock_read_accel(BNO055_AccelData_t *accel)
{
    accel->x = add_random_noise(BASE_ACCL_X, 0.1f);
    accel->y = add_random_noise(BASE_ACCL_Y, 0.1f);
    accel->z = add_random_noise(BASE_ACCL_Z, 0.5f);
    return 0;
}

static int mock_read_gyro(BNO055_GyroData_t *gyro)
{
    // Simulate a stationary sensor: near-zero angular velocity
    gyro->x = add_random_noise(BASE_GYRO_X, 0.01f);
    gyro->y = add_random_noise(BASE_GYRO_Y, 0.01f);
    gyro->z = add_random_noise(BASE_GYRO_Z, 0.01f);
    return 0;
}

static int mock_read_euler(BNO055_EulerData_t *euler)
{
    // Simulate a flat orientation, slightly tilted
    euler->heading = add_random_noise(BASE_EULER_H, 0.1f);
    euler->roll    = add_random_noise(BASE_EULER_R, 0.1f);
    euler->pitch   = add_random_noise(BASE_EULER_P, 0.1f);
    return 0;
}

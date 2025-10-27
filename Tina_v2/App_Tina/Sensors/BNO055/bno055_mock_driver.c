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
    // Simulate small random noise around 0, 0, +9.81 m/s^2 (gravity)
    accel->x = 0.05f;     // Small horizontal noise
    accel->y = -0.03f;    // Small horizontal noise
    accel->z = 9.81f;     // Gravity acting on Z-axis (stationary upright)
    return 0;
}

static int mock_read_gyro(BNO055_GyroData_t *gyro)
{
    // Simulate a stationary sensor: near-zero angular velocity
    gyro->x = 0.01f;  // degrees/sec
    gyro->y = -0.02f;
    gyro->z = 0.00f;
    return 0;
}

static int mock_read_euler(BNO055_EulerData_t *euler)
{
    // Simulate a flat orientation, slightly tilted
    euler->heading = 0.0f;   // Facing North (arbitrary)
    euler->roll    = 1.5f;   // Slight roll to the right (degrees)
    euler->pitch   = -0.8f;  // Slight nose down (degrees)
    return 0;
}

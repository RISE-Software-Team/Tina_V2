/* Host-only includes for building sensor host tests. This file pulls in
 * the sensor headers needed by mock drivers so the host toolchain has the
 * same type definitions (s8, struct bno055_t, etc.).
 */
#ifndef HOST_INCLUDES_H
#define HOST_INCLUDES_H

/* Include the BNO055 sensor header so the mock driver compiles on host. */
#include "../../Sensors/BNO055/bno055.h"

#endif /* HOST_INCLUDES_H */

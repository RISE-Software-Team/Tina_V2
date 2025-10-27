/* Minimal shim main.h for host testing (BNO055)
 * Provides minimal stubs for types and symbols expected by embedded code.
 */
#ifndef HOST_MAIN_H_BNO055
#define HOST_MAIN_H_BNO055

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Minimal stub for HAL I2C handle type used in the project (unused on host). */
typedef int I2C_HandleTypeDef;

/* Define BNO055_SUCCESS used by the mock driver */
#ifndef BNO055_SUCCESS
#define BNO055_SUCCESS 0
#endif

/* Error handler stub */
void Error_Handler(void);

#endif /* HOST_MAIN_H_BNO055 */

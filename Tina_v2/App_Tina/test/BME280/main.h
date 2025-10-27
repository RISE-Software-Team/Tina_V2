/* Minimal shim main.h for host testing
 * This header is placed in host_test/ and used only when building the
 * host-side test binary. It provides minimal stubs for types and symbols
 * that embedded code expects (HAL types, error handler, etc.).
 */
#ifndef HOST_MAIN_H
#define HOST_MAIN_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Minimal stub for HAL I2C handle type used in the project. */
typedef int I2C_HandleTypeDef;

/* Define BME280_OK used by the mock driver */
#ifndef BME280_OK
#define BME280_OK 0
#endif

/* Error handler stub */
void Error_Handler(void);

#endif /* HOST_MAIN_H */

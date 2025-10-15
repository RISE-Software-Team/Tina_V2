#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include "packet.h"
//--- LOGGER ---

#define DEBUG 1 // Radio used for debugging, allows for strings

#define ENABLE_SERIAL_LOG   0 // This used by logger to send serial data over uart pins

#if (DEBUG == 1)

#define MAX_PACKET_LENGTH 64

#else

#define MAX_PACKET_LENGTH 32

#endif

#define CRC_SIZE 1
#define LOG_HEADER_SIZE 2  // message code + message length
#define MAX_LOG_MESSAGE_LEN (MAX_PACKET_LENGTH - sizeof(HeaderPacket) - LOG_HEADER_SIZE - CRC_SIZE)

// --- FLIGHT STATE MACHINE CONFIG ---

// Launch detection
#define LAUNCH_ACCEL_THRESHOLD 3.0f     // Minimum acceleration to detect launch (g)
#define LAUNCH_ALTITUDE_THRESHOLD 10.0f // Minimum altitude to detect launch (m)

// Apogee detection
#define APOGEE_DELTA_H 2.0f              // Altitude change threshold for apogee (m)
#define APOGEE_SAMPLE_COUNT 5            // Number of samples to confirm apogee
#define APOGEE_VELOCITY_EPSILON 0.5f     // Velocity tolerance for apogee detection (m/s)
#define APOGEE_TIMEOUT_MS 120000         // Max time after launch for drogue (ms)

// Main chute deployment
#define MAIN_DEPLOY_ALTITUDE 100.0f      // Altitude to deploy main chute (m)

// Touchdown detection
#define TOUCHDOWN_ACCEL_TOLERANCE 0.5f   // Allowed deviation from 1g (g)
#define TOUCHDOWN_SAMPLE_COUNT 10        // Number of samples to confirm touchdown

// Preflight
#define PREFLIGHT_TIMEOUT_MS 300000      // Max time in preflight before error (ms)

#endif // GLOBAL_CONFIG_H

#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include "packet.h"
//--- LOGGER ---

#define DEBUG 1 // Radio used for debugging, allows for strings

#define ENABLE_SERIAL_LOG   0 // // This used by logger to send serial data over uart pins


#if (DEBUG == 1)

#define MAX_PACKET_LENGTH 64

#else

#define MAX_PACKET_LENGTH 32

#endif

#define CRC_SIZE 1
#define LOG_HEADER_SIZE 2  // message code + message length
#define MAX_LOG_MESSAGE_LEN (MAX_PACKET_LENGTH - sizeof(HeaderPacket) - LOG_HEADER_SIZE - CRC_SIZE)


// --- LAUNCH AND APOGEE DETECTION  ---

#define LAUNCH_ACCEL_THRESHOLD 3.0     // Minimum acceleration to detect launch (g)
#define LAUNCH_ALTITUDE_THRESHOLD 10.0  // Minimum altitude to detect launch (m)
#define APOGEE_TIMEOUT 120          // Max time after launch for drogue trigger (s)


// --- RECOVERY ---

#define MAIN_DEPLOY_ALTITUDE 100     // Altitude to deploy the main chute (m)


// --- TOUCHDOWN DETECTION CONFIG ---

#define TOUCHDOWN_ACCEL_TOLERANCE 0.5f  // Allowed deviation from -9.81 m/s² (m/s²)
#define TOUCHDOWN_STABLE_TIME 5.0f      // Duration acceleration must be stable to confirm touchdown (s)


#endif // GLOBAL_CONFIG_H

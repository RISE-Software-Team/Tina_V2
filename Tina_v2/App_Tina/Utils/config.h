#ifndef CONFIG_H
#define CONFIG_H


#include "packet.h"

//--- LOGGER ---

#define TLOG_DEBUG 1 // Radio used for debugging, allows for strings

//#define ENABLE_SERIAL_LOG 1 // This used by logger to send serial data over uart pins

#define LOG_INTERVAL_MS 1000

#ifdef TLOG_DEBUG
#define MAX_PACKET_LENGTH 64
#else
#define MAX_PACKET_LENGTH 32
#endif

#define CRC_SIZE 1
#define LOG_HEADER_SIZE 2  // message code + message length
#define MAX_LOG_MESSAGE_LEN (MAX_PACKET_LENGTH - sizeof(HeaderPacket) - LOG_HEADER_SIZE - CRC_SIZE)


// --- TELEMETRY ---

#define TELEMETRY_INTERVAL_MS 100


// --- LAUNCH AND APOGEE DETECTION  ---

#define LAUNCH_ACCEL_THRESHOLD 3.0              // Minimum acceleration to detect launch (g)
#define APOGEE_TIMEOUT 120                      // Max time after launch for drogue trigger (s)


// --- RECOVERY ---

#define MAIN_DEPLOY_ALTITUDE 250.0f             // Altitude to deploy the main chute (m)


// --- TOUCHDOWN DETECTION CONFIG ---

#define TOUCHDOWN_ACCEL_TOLERANCE 0.5f          // Allowed deviation from -9.81 m/s² (m/s²)
#define TOUCHDOWN_STABLE_TIME 5.0f              // Duration acceleration must be stable to confirm touchdown (s)

// New Stuff will later remove useless old configs
#define LAUNCH_ACCEL_THRESHOLD_MS2 30.0f        // Minimum acceleration to detect launch (m/s²)
# define TOUCHDOWN_ACCEL_TOLERANCE_MS2 0.5f     // ±0.5 m/s² around 9.81
#define LAUNCH_ALTITUDE_THRESHOLD_M 20.0        // Minimum altitude to detect launch (m)

#endif // GLOBAL_CONFIG_H

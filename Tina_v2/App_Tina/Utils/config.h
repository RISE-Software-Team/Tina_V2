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

// --- Flight FSM ---
#define HISTORY_SIZE 8

#define LAUNCH_ACCEL_THRESHOLD_MS2  30.0
#define LAUNCH_ALTITUDE_THRESHOLD_M 10.0

#define APOGEE_COUNTDOWN_SIZE 5
#define APOGEE_TIMEOUT_S      20

#define MAIN_DEPLOY_ALTITUDE_M 250.0

#endif // CONFIG_H

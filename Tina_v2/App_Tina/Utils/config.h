#ifndef CONFIG_H
#define CONFIG_H


#include "packet.h"

//--- LOGGER ---
 #define TLOG_DEBUG 1 // Radio used for debugging, allows for strings

//#define ENABLE_SERIAL_LOG 1 // This used by logger to send serial data over uart pins

#define LOG_INTERVAL_MS 1000

#define MAX_PACKET_LENGTH 64
#define CRC_SIZE 1
#define LOG_HEADER_SIZE 2  // message code + message length
#define MAX_LOG_MESSAGE_LEN (MAX_PACKET_LENGTH - sizeof(HeaderPacket) - LOG_HEADER_SIZE - CRC_SIZE)


// --- TELEMETRY ---
#define TELEMETRY_INTERVAL_MS 30

// --- Flight FSM ---
#define HISTORY_SIZE 50

#define LAUNCH_ACCEL_THRESHOLD_MS2 40
#define LAUNCH_ALTITUDE_THRESHOLD_M 5.6

#define APOGEE_COUNTDOWN_SIZE 30

#define DELTA_PRESSURE_FOR_APOGEE_DETECTION_PA 50

#define MAIN_DEPLOY_ALTITUDE_M 400

#define LAUNCH_TO_APOGEE_TIMEOUT_MS   15500 //
#define APOGEE_TO_MAIN_TIMEOUT_MS     22880 //

#endif // CONFIG_H

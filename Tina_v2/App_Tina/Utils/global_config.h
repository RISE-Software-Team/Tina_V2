/*
 * logger.h
 *
 *  Created on: Oct 7, 2025
 *      Author: krissal1234
 */

#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include "packet.h"
#include "types_support.h"


#define DEBUG 1                     // Radio used for debugging, allows for strings
#define ENABLE_SERIAL_LOG 0         // This used by logger to send serial data over uart pins

// Packet sizes based on debug mode
#if (DEBUG == 1)
#define MAX_PACKET_LENGTH 64        // Full debug packets
#else
#define MAX_PACKET_LENGTH 32        // Production mode (code only)
#endif

// Packet structure sizes
#define CRC_SIZE 1
#define LOG_HEADER_SIZE 2           // message code + message length
#define MAX_LOG_MESSAGE_LEN (MAX_PACKET_LENGTH - sizeof(HeaderPacket) - LOG_HEADER_SIZE - CRC_SIZE)

// ============================================================================
// SENSOR CONFIGURATION
// ============================================================================

// Main loop update rate (Hz)
#define FSM_UPDATE_RATE_HZ 20
#define FSM_UPDATE_PERIOD_MS (1000 / FSM_UPDATE_RATE_HZ)  // 50ms at 20Hz

// Sensor sample history size (for apogee/touchdown detection)
#define ALTITUDE_HISTORY_SIZE 3     // Keep last 3 altitude readings

// ============================================================================
// LAUNCH DETECTION
// ============================================================================

// Acceleration threshold: rocket must exceed this to detect launch
#define LAUNCH_ACCEL_THRESHOLD 3.0f  // g-forces

// Altitude threshold: OR condition with acceleration
#define LAUNCH_ALTITUDE_THRESHOLD 10.0f  // meters above ground

// ============================================================================
// APOGEE DETECTION
// ============================================================================

// Maximum altitude change allowed to consider apogee reached
#define APOGEE_DELTA_H 2.0f  // meters

// Number of consecutive samples needed to confirm apogee
#define APOGEE_SAMPLE_COUNT 5  // samples

// Vertical velocity must be within this range for apogee
#define APOGEE_VELOCITY_EPSILON 0.5f  // m/s

// Safety timeout: force drogue deployment if apogee not detected
#define APOGEE_TIMEOUT_MS 120000  // 120 seconds (2 minutes)

// ============================================================================
// PARACHUTE DEPLOYMENT
// ============================================================================

// Altitude to deploy main parachute (AGL - Above Ground Level)
#define MAIN_DEPLOY_ALTITUDE 100.0f  // meters

// Pyro channel fire duration
#define PYRO_FIRE_DURATION_MS 1000  // 1 second pulse

// ============================================================================
// TOUCHDOWN DETECTION
// ============================================================================

// Acceleration must be stable around 1g within this tolerance
#define TOUCHDOWN_ACCEL_TOLERANCE 0.5f  // g-forces

// Number of consecutive samples needed to confirm touchdown
#define TOUCHDOWN_SAMPLE_COUNT 10  // samples

// Altitude must be stable within this range
#define TOUCHDOWN_ALTITUDE_TOLERANCE 2.0f  // meters

// ============================================================================
// TIMEOUTS & SAFETY
// ============================================================================

// Maximum time allowed in preflight before error
#define PREFLIGHT_TIMEOUT_MS 300000  // 5 minutes

// Status report interval in preflight (low-rate telemetry)
#define PREFLIGHT_STATUS_INTERVAL_MS 5000  // 5 seconds

// Status report interval in recovery (low-rate telemetry)
#define RECOVERY_STATUS_INTERVAL_MS 10000  // 10 seconds

// General logging interval for non-critical states
#define GENERAL_LOG_INTERVAL_MS 1000  // 1 second

#endif // GLOBAL_CONFIG_H

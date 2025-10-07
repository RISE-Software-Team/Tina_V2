#include "error_handler.h"
#include "subghz_phy_app.h"
#include "packet.h"
#include <stdio.h>

// configuration flags
#define ENABLE_RADIO_SEND   1
#define ENABLE_SERIAL_LOG   1


#if ENABLE_RADIO_SEND
static void Radio_SendError(Severity_t severity, ErrorCode_t err_code, const char *msg) {

ErrorData_t err_packet;

err_packet.severity = severity;
err_packet.err_code = err_code;

Radio_SendError_Packet(&err_packet);

}
#endif

#if ENABLE_SERIAL_LOG
static void Log_Error(Severity_t severity, ErrorCode_t err_code, const char *msg)
{
    const char *sev_str = (severity == SEV_ERROR) ? "ERROR" : "WARNING";

    printf("[LOG] %s | Code: 0x%02X | %s\n", sev_str, err_code, msg);
}

#endif


void ErrorHandler_Report(Severity_t severity, ErrorCode_t err_code, const char *msg) {

#if ENABLE_RADIO_SEND
    Radio_SendError(severity, err_code, msg);
#endif
#if ENABLE_SERIAL_LOG
    Log_Error(severity, err_code, msg);
#endif
}

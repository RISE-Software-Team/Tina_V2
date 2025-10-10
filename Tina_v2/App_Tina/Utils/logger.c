#include <logger.h>
#include "subghz_phy_app.h"
#include "packet.h"
#include <stdio.h>
#include "global_config.h"

//Radio

void Radio_SendInfo(InfoCode_t info_code) {

InfoData_t info_packet;

info_packet.info_code = info_code;

Radio_SendInfo_Packet(&info_packet);

}


void ErrorHandler_Report(Severity_t severity, ErrorCode_t err_code, const char *msg)
{
    //radio log (no message)
#if defined(ENABLE_RADIO_LOG) && (ENABLE_RADIO_LOG == 1)
    {
        ErrorData_t err_packet;
        err_packet.severity = severity;
        err_packet.err_code = err_code;
        err_packet.message = NULL;  // no message
        Radio_SendError_Packet(&err_packet);
    }

    // skip debug log if radio log is enabled
#elif defined(ENABLE_RADIO_DEBUG) && (ENABLE_RADIO_DEBUG == 1)
    {
    	ErrorData_t debug_packet;
    	debug_packet.severity = severity;
    	debug_packet.err_code = err_code;
    	debug_packet.message = msg;

        Radio_SendDebug_Packet(&debug_packet);  // debug log includes message
    }
#endif


#if defined(ENABLE_SERIAL_LOG) && (ENABLE_SERIAL_LOG == 1)
    {
        const char *sev_str = (severity == SEV_ERROR) ? "ERROR" : "WARNING";
        printf("[LOG] %s | Code: 0x%02X | %s\n", sev_str, err_code, msg ? msg : "");
    }
#endif
}









/*
 * pyro_manager.c
 *
 * Created on: Oct 13, 2025
 * Author: krissal1234
 */
#include "main.h"
#include "pyro_manager.h"
#include <stdio.h>
#include "logger.h"
#include "config.h"
static uint8_t tx_data[2];

static int8_t send_command(uint8_t cmd, uint16_t max_cmd_wait_ms, uint8_t *response)
{

    tx_data[0] = cmd;
    tx_data[1] = 0x00;

    if (HAL_I2C_Master_Transmit(&hi2c2, PYRO_BOARD_ADDR, tx_data, 2, 100) != HAL_OK) {
        tlog(ERR_PYRO_SEND_COMMAND_FAIL, "I2C TX failed");
        return -1;
    }
    HAL_Delay(max_cmd_wait_ms);

    HAL_StatusTypeDef ret = HAL_I2C_Master_Receive(&hi2c2, PYRO_BOARD_ADDR, response, 2, max_cmd_wait_ms);

    if (ret != HAL_OK) {
        tlog(ERR_PYRO_RECEIVE_COMMAND_FAIL, "I2C RX timeout or failure");
        return -1;
    }

    return 0;
}


int8_t arm_pyros(uint8_t *response)
{

    send_command(CMD_ARM, 50, response);
    if(response[0] != TX_ACK) {
        return -1;
    }
    return 0;
}

int8_t deploy_parachute(Parachute_t type, uint8_t *response)
{
    int8_t status;
    char log_msg[MAX_LOG_MESSAGE_LEN];

    switch(type) {
        case DROGUE:
            status = send_command(CMD_FIRE_DROGUE, 200, response);

            if (status == 0 && !(response[1] & STATUS_DROGUE)) {
                snprintf(log_msg, sizeof(log_msg), "Drogue failed to fire (status: 0x%02X)", response[1]);
                tlog(ERR_PYRO_DROGUE_FAIL, log_msg);
                status = -1;
            }
            break;

        case MAIN:
            status = send_command(CMD_FIRE_MAIN, 800, response);

            if (status == 0) {
                uint8_t bits = response[1];

                if (!(bits & STATUS_MAIN)) {
                    snprintf(log_msg, sizeof(log_msg), "Main pyro failed (status: 0x%02X)", bits);
                    tlog(ERR_PYRO_MAIN_FAIL, log_msg);
                    status = -1;
                }
                if (!(bits & STATUS_BACKUP)) {
                    snprintf(log_msg, sizeof(log_msg), "Backup pyro failed (status: 0x%02X)", bits);
                    tlog(ERR_PYRO_BACKUP_FAIL, log_msg);
                    status = -1;
                }
            }
            break;

        default:
            snprintf(log_msg, sizeof(log_msg), "Unknown parachute type");
            tlog(INFO_DEBUG, log_msg);
            response[0] = 0xEE;
            response[1] = 0xEE;
            status = -1;
            break;
    }

    return status;
}



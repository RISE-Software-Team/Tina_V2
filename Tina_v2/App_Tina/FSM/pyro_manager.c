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

static uint8_t tx_data[2];

static int8_t send_command(uint8_t cmd, uint16_t max_cmd_wait_ms, uint8_t *response)
{

    tx_data[0] = cmd;
    tx_data[1] = 0x00;

    if (HAL_I2C_Master_Transmit(&hi2c2, PYRO_BOARD_ADDR, tx_data, 2, 100) != HAL_OK) {
        tlog(ERR_PYRO_SEND_COMMAND_FAIL, "I2C TX failed");
        return -1;
    }

    HAL_StatusTypeDef ret = HAL_I2C_Master_Receive(&hi2c2, PYRO_BOARD_ADDR, response, 2, max_cmd_wait_ms);

    if (ret != HAL_OK) {
        tlog(ERR_PYRO_RECEIVE_COMMAND_FAIL, "I2C RX timeout or failure");
        return -1;
    }

    return 0;
}


int8_t arm_pyros(uint8_t *response)
{
    return send_command(CMD_ARM, 50, response);
}

int8_t deploy_parachute(Parachute_t type, uint8_t *response)
{
    int8_t status;

    switch(type) {
        case DROGUE:
            status = send_command(CMD_FIRE_DROGUE, 50, response);
            break;
        case MAIN:
            // Longer timeout for main
            status = send_command(CMD_FIRE_MAIN, 150, response);
            break;
        default:
            tlog(INFO_DEBUG, "Unknown parachute type");
            response[0] = 0xEE;
            response[1] = 0xEE;
            status = -1;
            break;
    }
    return status;
}

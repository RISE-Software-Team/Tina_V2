/*
 * pyro_manager.c
 *
 *  Created on: Oct 13, 2025
 *      Author: krissal1234
 */
#include "main.h"
#include "pyro_manager.h"
#include <stdio.h>
#include "logger.h"

uint8_t tx_data[2];
uint8_t rx_data[2];
char msg[64];

void arm_pyros(){

	 tx_data[0] = CMD_ARM;
	 tx_data[1] = 0x00;

	 HAL_I2C_Master_Transmit(&hi2c2, PYRO_BOARD_ADDR, tx_data, 2, HAL_MAX_DELAY);
	 HAL_Delay(100);
	 HAL_I2C_Master_Receive(&hi2c2, PYRO_BOARD_ADDR, rx_data, 2, HAL_MAX_DELAY);

	 snprintf(msg, sizeof(msg), "ARM: ACK=%02X DATA=%02X\r\n", rx_data[0], rx_data[1]);
	 tlog(INFO_DEBUG, msg);
}

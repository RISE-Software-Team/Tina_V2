/*
 * pyro_manager.h
 *
 *  Created on: Oct 13, 2025
 *      Author: krissal1234
 */

#ifndef FSM_PYRO_MANAGER_H_
#define FSM_PYRO_MANAGER_H_

#include "main.h"

extern I2C_HandleTypeDef hi2c2;

#define PYRO_BOARD_ADDR (0x12 << 1)

#define CMD_ARM         0x01
#define CMD_FIRE_DROGUE 0x02
#define CMD_FIRE_MAIN   0x03
#define CMD_STATUS      0x04

typedef enum{
	DROGUE,
	MAIN
} Parachute_t;

int8_t arm_pyros(uint8_t *response);
int8_t deploy_parachute(Parachute_t type, uint8_t *response);

#endif /* FSM_PYRO_MANAGER_H_ */

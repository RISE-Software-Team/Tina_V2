/*
 * pyro_manager.h
 *
 *  Created on: Oct 13, 2025
 *      Author: krissal1234
 */

#ifndef FSM_PYRO_MANAGER_H_
#define FSM_PYRO_MANAGER_H_

extern I2C_HandleTypeDef hi2c2;

#define PYRO_BOARD_ADDR 0x42

#define CMD_ARM        0x01
#define CMD_FIRE       0x03
#define CMD_READ       0x04
#define CMD_STATUS     0x05

void arm_pyros();

#endif /* FSM_PYRO_MANAGER_H_ */

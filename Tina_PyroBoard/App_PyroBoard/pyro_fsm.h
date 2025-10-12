/*
 * pyro_fsm.h
 *
 *  Created on: Oct 11, 2025
 *      Author: krissal1234
 */

#ifndef PYRO_FSM_H_
#define PYRO_FSM_H_
#include "main.h"

typedef enum {
	STATE_UNARMED = 0,
	STATE_ARMED,
	STATE_FIRING,
	STATE_FAULT
} States_t;


typedef enum {
	PYRO_DROGUE = 1,
	PYRO_MAIN = 2,
	PYRO_BACKUP = 3,
	PYRO_CHAMBER = 4,
} PyroChannels_t;

typedef struct{
	uint8_t cmd;
	uint8_t param;
} CommandPacket_t;


#define PYRO_COUNT    4

//TODO maybe make it configurable
#define ENABLE_PYRO_DROGUE_PIN    En_Pyro_1_Pin
#define ENABLE_PYRO_DROGUE_PORT   En_Pyro_1_GPIO_Port

#define ENABLE_PYRO_MAIN_PIN      En_Pyro_2_Pin
#define ENABLE_PYRO_MAIN_PORT     En_Pyro_2_GPIO_Port

#define ENABLE_PYRO_BACKUP_PIN    En_Pyro_3_Pin
#define ENABLE_PYRO_BACKUP_PORT   En_Pyro_3_GPIO_Port

#define ENABLE_PYRO_CHAMBER_PIN   En_Pyro_4_Pin
#define ENABLE_PYRO_CHAMBER_PORT  En_Pyro_4_GPIO_Port


#define READ_PYRO_DROGUE_PIN      Pyro_Read_1_Pin
#define READ_PYRO_DROGUE_PORT     Pyro_Read_1_GPIO_Port

#define READ_PYRO_MAIN_PIN        Pyro_Read_2_Pin
#define READ_PYRO_MAIN_PORT       Pyro_Read_2_GPIO_Port

#define READ_PYRO_BACKUP_PIN      Pyro_Read_3_Pin
#define READ_PYRO_BACKUP_PORT     Pyro_Read_3_GPIO_Port

#define READ_PYRO_CHAMBER_PIN     Pyro_Read_4_Pin
#define READ_PYRO_CHAMBER_PORT    Pyro_Read_4_GPIO_Port



// I2C command IDs
#define CMD_ARM        0x01
#define CMD_FIRE       0x03
#define CMD_READ       0x04
#define CMD_STATUS     0x05


#define PYRO_DROGUE   1
#define PYRO_MAIN     2
#define PYRO_BACKUP   3
#define PYRO_CHAMBER  4

#define TX_ACK 0x00
#define TX_NACK 0xFF

//void pyro_set_state(py_state_t new_state);
void pyro_handle_command(CommandPacket_t *cmd, uint8_t *tx_buffer);
uint8_t pyro_read(PyroChannels_t pyro_channel);
void pyro_fire(uint8_t channel);
void pyro_disarm(void);
void pyro_arm(void);



#endif /* PYRO_FSM_H_ */

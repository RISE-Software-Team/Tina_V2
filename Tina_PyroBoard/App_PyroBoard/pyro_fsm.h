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
	PYRO_CHAMBER = 2,
	PYRO_MAIN = 3,
	PYRO_BACKUP = 4,
} PyroChannels_t;

typedef struct{
	uint8_t cmd;
	uint8_t param;
} CommandPacket_t;

//TODO maybe make it configurable
#define ENABLE_PYRO_DROGUE_PIN    En_Pyro_1_Pin
#define ENABLE_PYRO_DROGUE_PORT   En_Pyro_1_GPIO_Port

#define ENABLE_PYRO_CHAMBER_PIN   En_Pyro_2_Pin
#define ENABLE_PYRO_CHAMBER_PORT  En_Pyro_2_GPIO_Port

#define ENABLE_PYRO_MAIN_PIN      En_Pyro_3_Pin
#define ENABLE_PYRO_MAIN_PORT     En_Pyro_3_GPIO_Port

#define ENABLE_PYRO_BACKUP_PIN    En_Pyro_4_Pin
#define ENABLE_PYRO_BACKUP_PORT   En_Pyro_4_GPIO_Port

#define READ_PYRO_DROGUE_PIN      Pyro_Read_1_Pin
#define READ_PYRO_DROGUE_PORT     Pyro_Read_1_GPIO_Port

#define READ_PYRO_CHAMBER_PIN     Pyro_Read_2_Pin
#define READ_PYRO_CHAMBER_PORT    Pyro_Read_2_GPIO_Port

#define READ_PYRO_MAIN_PIN        Pyro_Read_3_Pin
#define READ_PYRO_MAIN_PORT       Pyro_Read_3_GPIO_Port

#define READ_PYRO_BACKUP_PIN      Pyro_Read_4_Pin
#define READ_PYRO_BACKUP_PORT     Pyro_Read_4_GPIO_Port

#define STATUS_DROGUE  (1 << 0)   // 0b00000001
#define STATUS_CHAMBER (1 << 1)   // 0b00000010
#define STATUS_MAIN    (1 << 2)   // 0b00000100
#define STATUS_BACKUP  (1 << 3)   // 0b00001000

#define CMD_ARM         0x01
#define CMD_FIRE_DROGUE 0x02
#define CMD_FIRE_MAIN   0x03
#define CMD_STATUS      0x04

#define TX_ACK 0xAA
#define TX_NACK 0xFF

void pyro_handle_command(CommandPacket_t *cmd, uint8_t *tx_buffer);
void pyro_arm(void);

#endif /* PYRO_FSM_H_ */
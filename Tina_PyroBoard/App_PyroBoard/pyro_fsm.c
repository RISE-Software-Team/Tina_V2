/*
 * pyro_fsm.c
 *
 *  Created on: Oct 11, 2025
 *      Author: krissal1234
 */

#include "pyro_fsm.h"
States_t system_state = STATE_UNARMED;


/* --------------------------- ARM --------------------------- */
void pyro_arm(void)
{
    // Check all pyro read pins are LOW
    if (HAL_GPIO_ReadPin(READ_PYRO_DROGUE_PORT, READ_PYRO_DROGUE_PIN) == GPIO_PIN_SET ||
        HAL_GPIO_ReadPin(READ_PYRO_MAIN_PORT, READ_PYRO_MAIN_PIN) == GPIO_PIN_SET ||
        HAL_GPIO_ReadPin(READ_PYRO_CHAMBER_PORT, READ_PYRO_CHAMBER_PIN) == GPIO_PIN_SET ||
        HAL_GPIO_ReadPin(READ_PYRO_BACKUP_PORT, READ_PYRO_BACKUP_PIN) == GPIO_PIN_SET)
    {
        system_state = STATE_FAULT;
        return;
    }

    system_state = STATE_ARMED;
}


void pyro_handle_command(CommandPacket_t *packet, uint8_t *tx_buffer)
{
    // defaults
    tx_buffer[0] = TX_NACK;
    tx_buffer[1] = 0x00;

    switch (packet->cmd)
    {
        case CMD_ARM:
            pyro_arm();
            tx_buffer[0] = (system_state == STATE_ARMED) ? TX_ACK : TX_NACK;
            break;

        case CMD_FIRE:
            pyro_fire(packet->param);
            tx_buffer[0] = TX_ACK;
            tx_buffer[1] = pyro_read(packet->param);
            break;

        case CMD_STATUS:
            tx_buffer[0] = TX_ACK;
            tx_buffer[1] = (uint8_t)system_state;
            break;
    }
}


uint8_t pyro_read(PyroChannels_t pyro_channel){

	GPIO_PinState pin_state;

	switch (pyro_channel)
	{
		case PYRO_DROGUE:
			pin_state = HAL_GPIO_ReadPin(READ_PYRO_DROGUE_PORT, READ_PYRO_DROGUE_PIN);
			break;

		case PYRO_MAIN:
			pin_state = HAL_GPIO_ReadPin(READ_PYRO_MAIN_PORT, READ_PYRO_MAIN_PIN);
			break;

		case PYRO_BACKUP:
			pin_state = HAL_GPIO_ReadPin(READ_PYRO_BACKUP_PORT, READ_PYRO_BACKUP_PIN);
			break;

		case PYRO_CHAMBER:
			pin_state = HAL_GPIO_ReadPin(READ_PYRO_CHAMBER_PORT, READ_PYRO_CHAMBER_PIN);
			break;

		default:
			return TX_NACK;
	}

	return (pin_state == GPIO_PIN_SET) ? 1 : 0;

}

void pyro_fire(uint8_t channel)
{

    if(system_state != STATE_ARMED)
    {
        system_state = STATE_FAULT;
        return;
    }

    system_state = STATE_FIRING;

    switch(channel)
    {
        case PYRO_DROGUE:    HAL_GPIO_WritePin(ENABLE_PYRO_DROGUE_PORT, ENABLE_PYRO_DROGUE_PIN, GPIO_PIN_SET);
                           break;

        case PYRO_MAIN:    HAL_GPIO_WritePin(ENABLE_PYRO_MAIN_PORT, ENABLE_PYRO_MAIN_PIN, GPIO_PIN_SET);
                           break;

        case PYRO_BACKUP:  HAL_GPIO_WritePin(ENABLE_PYRO_BACKUP_PORT, ENABLE_PYRO_BACKUP_PIN, GPIO_PIN_SET);
                           break;

        case PYRO_CHAMBER: HAL_GPIO_WritePin(ENABLE_PYRO_CHAMBER_PORT, ENABLE_PYRO_CHAMBER_PIN, GPIO_PIN_SET);
                           break;

        default:           system_state = STATE_FAULT;
//                           all_pyros_off();
                           return;
    }

    system_state = STATE_ARMED;
}

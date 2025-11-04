#include "pyro_fsm.h"
#include "main.h"

#define FIRE_DELAY_MS 150

States_t system_state = STATE_UNARMED;

//bitfied to track which pyros have been fired
// Bit 0: DROGUE, Bit 1: CHAMBER, Bit 2: MAIN, Bit 3: BACKUP

static uint8_t pyro_fire_channel(PyroChannels_t channel);

static uint8_t pyro_verify_fired(PyroChannels_t channel);
uint8_t pyro_status_bits = 0;

void pyro_arm(void)
{
    system_state = STATE_ARMED;
    if (HAL_GPIO_ReadPin(READ_PYRO_DROGUE_PORT, READ_PYRO_DROGUE_PIN) == GPIO_PIN_RESET ||
        HAL_GPIO_ReadPin(READ_PYRO_MAIN_PORT, READ_PYRO_MAIN_PIN) == GPIO_PIN_RESET ||
//        HAL_GPIO_ReadPin(READ_PYRO_CHAMBER_PORT, READ_PYRO_CHAMBER_PIN) == GPIO_PIN_RESET ||
        HAL_GPIO_ReadPin(READ_PYRO_BACKUP_PORT, READ_PYRO_BACKUP_PIN) == GPIO_PIN_RESET)
    {
        // system_state = STATE_FAULT;
        // return;
    }

    // system_state = STATE_ARMED;
}

void pyro_handle_command(CommandPacket_t *packet, uint8_t *tx_buffer)
{
    tx_buffer[0] = TX_NACK;
    tx_buffer[1] = pyro_status_bits;

    switch(packet->cmd)
    {
        case CMD_ARM:
            pyro_arm();
            tx_buffer[0] = (system_state == STATE_ARMED) ? TX_ACK : TX_NACK;
            pyro_status_bits = 0; //resettign bitfield
            break;

        case CMD_FIRE_DROGUE:
            if(system_state == STATE_ARMED)
            {
                uint8_t fired = pyro_fire_channel(PYRO_DROGUE);
                tx_buffer[0] = fired ? TX_ACK : TX_NACK;
            }
            break;

        case CMD_FIRE_MAIN:
            if(system_state == STATE_ARMED)
            {
//                pyro_fire_channel(PYRO_CHAMBER);
//                HAL_Delay(FIRE_DELAY_MS); // chamber was removed
                pyro_fire_channel(PYRO_MAIN);
                HAL_Delay(FIRE_DELAY_MS);
                pyro_fire_channel(PYRO_BACKUP);

                uint8_t expected_bits = STATUS_MAIN | STATUS_BACKUP;
                tx_buffer[0] = ((pyro_status_bits & expected_bits) == expected_bits) ? TX_ACK : TX_NACK;
            }
            break;

        case CMD_STATUS:
            tx_buffer[0] = TX_ACK;
            break;

        default:
            system_state = STATE_FAULT;
            break;
    }

    tx_buffer[1] = pyro_status_bits;
}


static uint8_t pyro_fire_channel(PyroChannels_t channel)
{
    GPIO_TypeDef *port = NULL;
    uint16_t pin = 0;

    switch(channel)
    {
        case PYRO_DROGUE:  port = ENABLE_PYRO_DROGUE_PORT; pin = ENABLE_PYRO_DROGUE_PIN; break;
//        case PYRO_CHAMBER: port = ENABLE_PYRO_CHAMBER_PORT; pin = ENABLE_PYRO_CHAMBER_PIN; break;
        case PYRO_MAIN:    port = ENABLE_PYRO_MAIN_PORT;    pin = ENABLE_PYRO_MAIN_PIN;    break;
        case PYRO_BACKUP:  port = ENABLE_PYRO_BACKUP_PORT;  pin = ENABLE_PYRO_BACKUP_PIN;  break;
        default:
            system_state = STATE_FAULT;
            return 0;
    }

    system_state = STATE_FIRING;

    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    HAL_Delay(FIRE_DELAY_MS);
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    HAL_Delay(FIRE_DELAY_MS);



    uint8_t fired = pyro_verify_fired(channel);
    if(fired)
    {
        switch(channel)
        {
            case PYRO_DROGUE:  pyro_status_bits |= STATUS_DROGUE; break;
//            case PYRO_CHAMBER: pyro_status_bits |= STATUS_CHAMBER; break;
            case PYRO_MAIN:    pyro_status_bits |= STATUS_MAIN;    break;
            case PYRO_BACKUP:  pyro_status_bits |= STATUS_BACKUP;  break;
        }
    }

    system_state = STATE_ARMED;
    return fired;
}



static uint8_t pyro_verify_fired(PyroChannels_t channel)
{
    GPIO_TypeDef *port;
    uint16_t pin;

    switch (channel)
    {
        case PYRO_DROGUE:
            port = READ_PYRO_DROGUE_PORT;
            pin  = READ_PYRO_DROGUE_PIN;
            break;
//        case PYRO_CHAMBER:
//            port = READ_PYRO_CHAMBER_PORT;
//            pin  = READ_PYRO_CHAMBER_PIN;
//            break;
        case PYRO_MAIN:
            port = READ_PYRO_MAIN_PORT;
            pin  = READ_PYRO_MAIN_PIN;
            break;
        case PYRO_BACKUP:
            port = READ_PYRO_BACKUP_PORT;
            pin  = READ_PYRO_BACKUP_PIN;
            break;
        default:
            return 0;
    }


    GPIO_PinState state = HAL_GPIO_ReadPin(port, pin);

    return state == GPIO_PIN_RESET;
}

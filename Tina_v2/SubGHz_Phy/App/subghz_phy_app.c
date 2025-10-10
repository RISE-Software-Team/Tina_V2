/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "sys_app.h"

#include "subghz_phy_app.h"
#include "radio.h"

/* USER CODE BEGIN Includes */
#include "stm32_seq.h"
#include "global_config.h"
#include "stm32_timer.h"
#include <stdio.h>

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



/*Size of the payload to be sent*/
#define CFG_SEQ_Task_SubGHz_Phy_App_Process 0  // pick an unused task slot

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */

static uint8_t BufferTx[MAX_PACKET_LENGTH];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */



/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */

	  /* Radio initialization */
	  RadioEvents.TxDone = OnTxDone;
	  RadioEvents.RxDone = OnRxDone;
	  RadioEvents.TxTimeout = OnTxTimeout;
	  RadioEvents.RxTimeout = OnRxTimeout;
	  RadioEvents.RxError = OnRxError;

	  Radio.Init(&RadioEvents);
  /* USER CODE END SubghzApp_Init_1 */

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */
    Radio.SetChannel(RF_FREQUENCY);

    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                      LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, 0, 0, LORA_IQ_INVERSION_ON, true);


	Radio.SetMaxPayloadLength(MODEM_LORA, MAX_PACKET_LENGTH);
	Radio.SetPublicNetwork(false);

	memset(BufferTx, 0x0, MAX_PACKET_LENGTH);

	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);


  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */


void SubghzApp_Process(void)
{
	// This function handles all radio event callbacks and state transitions.
	// It MUST be called frequently from the main application loop.
	Radio.IrqProcess();
}


void subghz_send_telemetry_packet(const TelemetryPacket_t *telemetry_packet)
{
    if (!telemetry_packet) {
        tlog(ERR_MISC_ERR, "Telemetry pointer NULL");
        return;
    }

    uint8_t len = packet_build_telemetry(BufferTx, telemetry_packet);

    if (len == 0) {
        tlog(ERR_MISC_ERR, "Telemetry packet build failed");
        return;
    }

    if (len > MAX_PACKET_LENGTH) {
        tlog(ERR_MISC_ERR, "Telemetry packet overflow");
        return;
    }

    radio_status_t send_status = Radio.Send(BufferTx, len);

    if (send_status != RADIO_STATUS_OK){
        tlog(ERR_MISC_ERR, "Radio send failed");
    }
}

void subghz_send_log_packet(const LogPacket_t *log_packet)
{
    if (!log_packet) {
        tlog(ERR_MISC_ERR, "Log packet pointer NULL");
        return;
    }
    uint8_t len = packet_build_log(BufferTx, log_packet);

    if (len > MAX_PACKET_LENGTH) {
        tlog(ERR_MISC_ERR, "Error packet overflow");
        return;
    }

    radio_status_t send_status = Radio.Send(BufferTx, len);

    if (send_status != RADIO_STATUS_OK){
        tlog(ERR_MISC_ERR, "Radio send failed");
    }


}


/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */

  /* USER CODE END OnTxDone */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
  /* USER CODE END OnRxDone */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  /* USER CODE END OnTxTimeout */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  /* USER CODE END OnRxError */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

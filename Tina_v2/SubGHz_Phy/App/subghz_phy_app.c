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

#include "stm32_timer.h"
#include <stdio.h>
#include "packet.h"

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

#define MAX_APP_BUFFER_SIZE 32 //safe upper boud
#define CFG_SEQ_Task_SubGHz_Phy_App_Process 0  // pick an unused task slot

//Packet Types
#define PACKET_HEADER 0xAA
#define MAX_PACKET_SIZE 29 //max total size of packet (header + payload + checksum)
#define TELEMETRY_PAYLOAD_SIZE 28 //fixed size telemetry packet (its the only packet that has fixed size as the others may have text)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */

static uint8_t BufferTx[MAX_APP_BUFFER_SIZE];

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


void Radio_SendInfo(void);
void radio_SendErr(void);

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


	Radio.SetMaxPayloadLength(MODEM_LORA, MAX_APP_BUFFER_SIZE);
	Radio.SetPublicNetwork(false);

	memset(BufferTx, 0x0, MAX_APP_BUFFER_SIZE);

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


void Radio_SendTelemetry_Packet(const TelemetryData_t *telemetry)
{
    if (!telemetry) {
        ErrorHandler_Report(SEV_ERROR, ERR_MISC_ERR, "Telemetry pointer NULL");
        return;
    }

    uint8_t len = Packet_BuildTelemetry(BufferTx, telemetry);

    if (len == 0) {
        ErrorHandler_Report(SEV_ERROR, ERR_MISC_ERR, "Telemetry packet build failed");
        return;
    }

    if (len > MAX_APP_BUFFER_SIZE) {
        ErrorHandler_Report(SEV_ERROR, ERR_MISC_ERR, "Telemetry packet overflow");
        return;
    }

    radio_status_t send_status = Radio.Send(BufferTx, len);
    if (send_status != RADIO_STATUS_OK){
        ErrorHandler_Report(SEV_WARNING, ERR_MISC_ERR, "Radio send failed");
    }
}

void Radio_SendError_Packet(const ErrorData_t *err_packet)
{
    if (!err_packet) {
        ErrorHandler_Report(SEV_ERROR, ERR_MISC_ERR, "Error packet pointer NULL");
        return;
    }

    uint8_t len = Packet_BuildError(BufferTx, err_packet);

    if (len == 0) {
        ErrorHandler_Report(SEV_ERROR, ERR_MISC_ERR, "Error packet build failed");
        return;
    }

    if (len > MAX_APP_BUFFER_SIZE) {
        ErrorHandler_Report(SEV_ERROR, ERR_MISC_ERR, "Error packet overflow");
        return;
    }

    bool success = Radio.Send(BufferTx, len);
    if (!success) {
        ErrorHandler_Report(SEV_WARNING, ERR_MISC_ERR, "Radio send failed");
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

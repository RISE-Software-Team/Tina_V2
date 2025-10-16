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

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
static TxQueue_t txQueue;
static bool txBusy = false;


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


/* USER CODE END PV */

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

	memset(&txQueue, 0, sizeof(txQueue));
  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

static bool enqueue_packet(TxQueueItem_t packet) {
    if (txQueue.count >= TX_QUEUE_SIZE) return false;
    txQueue.items[txQueue.tail] = packet;
    txQueue.tail = (txQueue.tail + 1) % TX_QUEUE_SIZE; //we use modulus to make sure it wraps around
    txQueue.count++;
    return true;
}

static bool dequeue_packet(TxQueueItem_t *packet) {
    if (txQueue.count == 0) return false;
    *packet = txQueue.items[txQueue.head];
    txQueue.head = (txQueue.head + 1) % TX_QUEUE_SIZE;
    txQueue.count--;
    return true;
}

void radio_send_packet()
{
    if (txBusy) return;
    if (txQueue.count == 0) return;

    TxQueueItem_t packet;
    if (!dequeue_packet(&packet)) return;

    if (packet.length == 0 || packet.length > MAX_PACKET_LENGTH) return;

    if (Radio.Send(packet.payload, packet.length) == RADIO_STATUS_OK) {
        txBusy = true;
    } else {
        txBusy = false;
    }
}

void radio_send_telemetry_packet(TelemetryPacket_t telemetry)
{
	TxQueueItem_t item;

	item.length = packet_build_telemetry(item.payload, telemetry);
	if(item.length == 0 || item.length > MAX_PACKET_LENGTH)
		return;

	enqueue_packet(item);
	radio_send_packet();
}

void radio_send_log_packet(LogPacket_t log)
{
    TxQueueItem_t item;

    item.length = packet_build_log(item.payload, log);
    if(item.length == 0 || item.length > MAX_PACKET_LENGTH)
    	return;

    enqueue_packet(item);
	radio_send_packet();
}

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
	txBusy = false;
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
	txBusy = false;
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

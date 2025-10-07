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
#include <stdio.h> // <--- FIX 1: REQUIRED FOR printf AND fflush(stdout)

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
#define TELEMETRY_PAYLOAD_SIZE 28 // fixed size telemetry packet (its the only packet that has fixed size as the others may have text)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */

static uint8_t BufferTx[MAX_APP_BUFFER_SIZE];
static uint16_t seq_num = 0;
static UTIL_TIMER_Object_t txLedTimer;  // one-shot timer for TX LED

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
static void OnTxLedEvent(void *context);
static uint8_t BuildPacketHeader(uint8_t *buffer, PacketType_t type);
static void Pack16(uint8_t *buf, uint16_t value);
static void Pack32(uint8_t *buf, uint32_t value);
static uint8_t CalculateChecksum(const uint8_t *buffer, uint8_t len);


static uint8_t BuildTelemetryPacket(uint8_t *buffer, const TelemetryData_t *data);
void Radio_SendTelemetry(int16_t acc_x, int16_t acc_y, int16_t acc_z,
                   int16_t gyro_x, int16_t gyro_y, int16_t gyro_z,
                   uint32_t altitude, uint8_t event_flags, uint8_t sys_state);

void Radio_SendInfo(void);
void radio_SendErr(void);
/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */
//	UTIL_TIMER_Create(&txLedTimer, TX_LED_BLINK_MS, UTIL_TIMER_ONESHOT, OnTxLedEvent, NULL);
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
  	Radio.SetPublicNetwork(false);  // false = private syncword, true = public (LoRaWAN)
  /*fills tx buffer*/
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
static uint8_t BuildPacketHeader(uint8_t *buffer, PacketType_t type)
{
    uint8_t idx = 0;
    buffer[idx++] = PACKET_HEADER;

	//seq number is 2 bytes so we need to split it into 2 bytes
    buffer[idx++] = (uint8_t)((seq_num >> 8) & 0xFF);
    buffer[idx++] = (uint8_t)(seq_num & 0xFF);
    seq_num++; //TODO decide if this goes here or after its sent

    idx+=1; // for packet length
    uint32_t ts = HAL_GetTick();  //TODO set this to measure time since launch not since boot like its currently doing
    buffer[idx++] = (uint8_t)((ts >> 24) & 0xFF);
    buffer[idx++] = (uint8_t)((ts >> 16) & 0xFF);
    buffer[idx++] = (uint8_t)((ts >> 8) & 0xFF);
    buffer[idx++] = (uint8_t)(ts & 0xFF);

    buffer[idx++] = type;

    return idx; //indicates start of payload
}


static uint8_t BuildTelemetryPacket(uint8_t *buffer, const TelemetryData_t *data){// in big endian order

	uint8_t idx = BuildPacketHeader(buffer, PACKET_TYPE_TELEMETRY);

	Pack16(&buffer[idx], data->acc_x); idx += 2;
	Pack16(&buffer[idx], data->acc_y); idx += 2;
	Pack16(&buffer[idx], data->acc_z); idx += 2;

	Pack16(&buffer[idx], data->gyro_x); idx += 2;
	Pack16(&buffer[idx], data->gyro_y); idx += 2;
	Pack16(&buffer[idx], data->gyro_z); idx += 2;

	Pack32(&buffer[idx], data->altitude); idx += 4;

	buffer[idx++] = data->event_flags;
	buffer[idx++] = data->sys_state;


	uint8_t crc = CalculateChecksum(buffer, idx);
	buffer[idx++] = crc;

    buffer[3] = idx; // 3 is PACKET_LEN in header

    return idx;
}


void Radio_SendTelemetry(int16_t acc_x, int16_t acc_y, int16_t acc_z,
                   int16_t gyro_x, int16_t gyro_y, int16_t gyro_z,
                   uint32_t altitude, uint8_t event_flags, uint8_t sys_state)
{


    TelemetryData_t telemetry;

    telemetry.acc_x = acc_x;
    telemetry.acc_y = acc_y;
    telemetry.acc_z = acc_z;
    telemetry.gyro_x = gyro_x;
    telemetry.gyro_y = gyro_y;
    telemetry.gyro_z = gyro_z;
    telemetry.altitude = altitude;
    telemetry.event_flags = event_flags;
    telemetry.sys_state = sys_state;

    uint8_t len = BuildTelemetryPacket(BufferTx, &telemetry);



     uint8_t testBuffer[5] = {0xAA, 0x01, 0x02, 0x03, 0x04};
     Radio.Send(testBuffer, 5);
     printf("sent");
     fflush(stdout);

//    if (len > 0)
//    {
//        Radio.Send(BufferTx, len);
//    }
//    else
//    {
//        printf("ERROR: Packet build failed (Len 0)\r\n");
//        fflush(stdout);
//    }

}


//simple XOR checksum implementation
static uint8_t CalculateChecksum(const uint8_t *buffer, uint8_t len)
{
    uint8_t crc = 0;

    for (uint8_t i = 1u; i < len; ++i)
    {
        crc ^= buffer[i];
    }
    return crc;
}

//helper functions to split into bytes
static void Pack16(uint8_t *buf, uint16_t value)
{
    buf[0] = (value >> 8) & 0xFF;
    buf[1] = value & 0xFF;
}

static void Pack32(uint8_t *buf, uint32_t value)
{
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8)  & 0xFF;
    buf[3] = value & 0xFF;
}

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
	printf("TX Done (Seq: %d)\r\n", seq_num);
	fflush(stdout);
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

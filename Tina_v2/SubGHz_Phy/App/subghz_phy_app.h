/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.h
  * @author  MCD Application Team
  * @brief   Header of application of the SubGHz_Phy Middleware
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SUBGHZ_PHY_APP_H__
#define __SUBGHZ_PHY_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct {
	int16_t acc_x;
	int16_t acc_y;
	int16_t acc_z;
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
	int32_t altitude;
	uint8_t event_flags;
	uint8_t sys_state;

} TelemetryData_t;

typedef enum {
	  PACKET_TYPE_TELEMETRY = 0x01,
	  PACKET_TYPE_INFO      = 0x02,
	  PACKET_TYPE_ERROR     = 0x03
} PacketType_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */
#define LED_TX_GPIO_Port GPIOB
#define LED_TX_Pin GPIO_PIN_5
#define TX_LED_BLINK_MS 100

#define TX_TIMEOUT_VALUE 							3000 //TODO find a suitable time for this
#define RX_TIMEOUT_VALUE              				3000
#define RF_FREQUENCY                                868000000 /* Hz  */
#define TX_OUTPUT_POWER                             14        /* dBm */
#define LORA_BANDWIDTH                              0         /* [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved] */
#define LORA_SPREADING_FACTOR                       7         /* [SF7..SF12] */
#define LORA_CODINGRATE                             1         /* [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8] */
#define LORA_PREAMBLE_LENGTH                        8         /* Same for Tx and Rx */
#define LORA_SYMBOL_TIMEOUT                         5         /* Symbols */
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Init Subghz Application
  */
void SubghzApp_Init(void);

/* USER CODE BEGIN EFP */
void SubghzApp_Process(void);

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__SUBGHZ_PHY_APP_H__*/

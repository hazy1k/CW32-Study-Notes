/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 CW.
  * All rights reserved.</center></h2>
  *
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "base_types.h"
#include "cw32f030.h"
#include "system_cw32f030.h"
#include "interrupts_cw32f030.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */


/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* USER CODE END ET */


/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* USER CODE END EC */


/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* 序列通道连续采样模式 */
//#define SQR_CONTINUOUS_MODE_EN
/* 序列通道扫描采样模式 */
//#define SQR_SCAN_MODE_EN
/* 序列通道多次采样模式 */
#define SQR_MORE_MODE_EN              
/* 序列通道断续采样模式 */
//#define SQR_BREAK_MODE_EN
/* USER CODE END EM */


/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */
void LED_Init(void);
/* USER CODE END EFP */


/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT CW *****END OF FILE****/

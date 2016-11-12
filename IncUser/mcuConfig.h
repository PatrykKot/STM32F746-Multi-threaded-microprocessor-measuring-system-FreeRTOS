/*
 * mcuConfig.h
 *
 *  Created on: Jul 5, 2016
 *      Author: Patryk
 */

#ifndef MCUCONFIG_H_
#define MCUCONFIG_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "stm32746g_discovery_lcd.h"

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c3;

LTDC_HandleTypeDef hltdc;

SAI_HandleTypeDef hsai_BlockA2;
SAI_HandleTypeDef hsai_BlockB2;

SDRAM_HandleTypeDef hsdram1;

void SystemClock_Config(void);
void Error_Handler(void);
void MX_GPIO_Init(void);
void MX_DMA2D_Init(void);
void MX_FMC_Init(void);
void MX_I2C3_Init(void);
void MX_LTDC_Init(void);
void MX_SAI2_Init(void);

#endif /* MCUCONFIG_H_ */

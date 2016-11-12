/*
 * usrTaskSupport.h
 *
 *  Created on: 14 wrz 2016
 *      Author: Patryk
 */

#ifndef USRTASKSUPPORT_H_
#define USRTASKSUPPORT_H_

#include "cmsis_os.h"
#include "stdio.h"
#include "lcdLogger.h"

uint8_t isTaskNull(osThreadId task);
uint8_t isPoolNull(osPoolId pool);
uint8_t isMutexNull(osMutexId mutex);
uint8_t isMailQueueNull(osMailQId mailQueue);
void printHandleNull(char* taskName);
void printHandleOk(char* taskName);
uint8_t checkTaskHandle(osThreadId task, char* taskName);
uint8_t checkMutexHandle(osMutexId mutex, char* mutexName);
uint8_t checkMailQueueHandle(osMailQId mailQueue, char* mailQueueName);
uint8_t checkPoolHandle(osPoolId pool, char* poolName);

#endif /* USRTASKSUPPORT_H_ */

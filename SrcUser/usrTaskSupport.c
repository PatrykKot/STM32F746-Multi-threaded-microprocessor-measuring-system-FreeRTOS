/*
 * usrTaskSupport.c
 *
 *  Created on: 14 wrz 2016
 *      Author: Patryk
 */

#include "usrTaskSupport.h"

void printNullHandle(char* handleName) {
	char msg[30];
	sprintf(msg, "%s is NULL", handleName);
	logErr(msg);
}

void printHandleOk(char* handleName) {
	char msg[30];
	sprintf(msg, "%s is OK", handleName);
	logMsg(msg);
}

uint8_t checkTaskHandle(osThreadId task, char* taskName) {
	if (isTaskNull(task)) {
		printNullHandle(taskName);
		return 1;
	} else {
		printHandleOk(taskName);
		return 0;
	}
}

uint8_t checkMutexHandle(osMutexId mutex, char* mutexName) {
	if (isMutexNull(mutex)) {
		printNullHandle(mutexName);
		return 1;
	} else {
		printHandleOk(mutexName);
		return 0;
	}
}

uint8_t checkMailQueueHandle(osMailQId mailQueue, char* mailQueueName) {
	if (isMailQueueNull(mailQueue)) {
		printNullHandle(mailQueueName);
		return 1;
	} else {
		printHandleOk(mailQueueName);
		return 0;
	}
}

uint8_t checkPoolHandle(osPoolId pool, char* poolName) {
	if (isPoolNull(pool)) {
		printNullHandle(poolName);
		return 1;
	} else {
		printHandleOk(poolName);
		return 0;
	}
}


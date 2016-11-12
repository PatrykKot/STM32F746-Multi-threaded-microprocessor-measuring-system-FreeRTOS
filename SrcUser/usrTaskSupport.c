/*
 * usrTaskSupport.c
 *
 *  Created on: 14 wrz 2016
 *      Author: Patryk
 */

#include "usrTaskSupport.h"

uint8_t isTaskNull(osThreadId task) {
	if (task == NULL)
		return 1;
	return 0;
}

uint8_t isPoolNull(osPoolId pool) {
	if (pool == NULL)
		return 1;
	return 0;
}

uint8_t isMutexNull(osMutexId mutex) {
	if (mutex == NULL)
		return 1;
	return 0;
}

uint8_t isMailQueueNull(osMailQId mailQueue) {
	if (mailQueue == NULL)
		return 1;
	return 0;
}

void printHandleNull(char* handleName) {
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
		printHandleNull(taskName);
		return 1;
	} else {
		printHandleOk(taskName);
		return 0;
	}
}

uint8_t checkMutexHandle(osMutexId mutex, char* mutexName) {
	if (isMutexNull(mutex)) {
		printHandleNull(mutexName);
		return 1;
	} else {
		printHandleOk(mutexName);
		return 0;
	}
}

uint8_t checkMailQueueHandle(osMailQId mailQueue, char* mailQueueName) {
	if (isMailQueueNull(mailQueue)) {
		printHandleNull(mailQueueName);
		return 1;
	} else {
		printHandleOk(mailQueueName);
		return 0;
	}
}

uint8_t checkPoolHandle(osPoolId pool, char* poolName) {
	if (isPoolNull(pool)) {
		printHandleNull(poolName);
		return 1;
	} else {
		printHandleOk(poolName);
		return 0;
	}
}


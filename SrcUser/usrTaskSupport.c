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

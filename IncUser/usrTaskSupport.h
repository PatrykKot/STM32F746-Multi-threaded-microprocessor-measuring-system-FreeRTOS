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

/* Functions */
void printNullHandle(char* taskName);
void printHandleOk(char* taskName);

#endif /* USRTASKSUPPORT_H_ */

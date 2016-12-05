/*
 * usrTasks.h
 *
 *  Created on: 14 wrz 2016
 *      Author: Patryk Kotlarz
 */

#ifndef USRTASKS_H_
#define USRTASKS_H_

#include "lcdLogger.h"
#include "ethernetLib.h"
#include "audioRecording.h"
#include "soundProcessing.h"
#include "lcdAmplitudePrinter.h"
#include "ethernetLib.h"
#include "mcuConfig.h"
#include "jsonConfiguration.h"

#include "usrTaskSupport.h"

#include "cmsis_os.h"
#include "lwip.h"
/* Functions */
void threadsInit();
void osCheck();

/* OS threads */
#ifdef LCD_PRINTER_SUPPORT
void lcdTask(void const * argument);
#endif

void soundProcessingTask(void const * argument);
void samplingTask(void const * argument);
void dhcpTask(void const * argument);
void streamingTask(void const * argument);
void httpConfigTask(void const * argument);
void initTask(void const * argument);

/* Delays */
#ifdef LCD_PRINTER_SUPPORT
#define LCD_TASK_DELAY_TIME 100
#endif

#define INIT_TASK_DELAY_TIME 5000
#define ETHERNET_TASK_DELAY_TIME 1000
#define CONNECTION_TASK_DELAY_TIME 10
#define HTTP_CONFIG_TASK_DELAY_TIME 100

/* Timeouts */
#define HTTP_HOST_ACCEPT_TIMEOUT 1
#define HTTP_RECEIVE_TIMEOUT 1500

/* Other */
#define MAXIMUM_DMA_AUDIO_MESSAGE_QUEUE_SIZE 8

/* Signals */
#define DHCP_FINISHED_SIGNAL 0x0001
#define START_SOUND_PROCESSING_SIGNAL 0x0001

#endif /* USRTASKS_H_ */

/*
 * freeRtosSystemInfoSupport.c
 *
 *  Created on: 1 sty 2017
 *      Author: Patryk
 */

#include "freeRtosSystemInfoSupport.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mcuConfig.h"

volatile uint16_t tim6OverflowCount;

void getTaskUsageDetails(char* detailStr) {
	vTaskGetRunTimeStats(detailStr);
}

void configureTimerForRuntimestats() {

}

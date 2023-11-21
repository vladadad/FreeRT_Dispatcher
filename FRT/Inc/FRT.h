/*
 * FRT.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Vlad
 */

#ifndef INC_FRT_H_
#define INC_FRT_H_

#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "cmsis_os.h"
//#include "task.h"
//#include "timers.h"
//#include "queue.h"
//#include "semphr.h"
//#include "event_groups.h"
//#include "task.h"
//#include "FreeRTOS.h"
//#include "projdefs.h"
//#include "FreeRTOSConfig.h"

extern UART_HandleTypeDef huart3;
#define UART_DEBUG &huart3


// Define constants for special characters
#define END_OF_STRING 0
#define BACKSPACE 8
#define LINE_FEED 10
#define CARRIAGE_RETURN 13
#define NORMAL 0
#define ABOVE_NORMAL 1
#define HIGH 2
#define MIN 100000
#define MAX 200000
#define SMALL_TASK_SIZE	((uint16_t)200)
#define LARGE_TASK_SIZE ((uint16_t)256)


void main_FRT();
void DispatcherTask(void const * argument);
void PoliceDepartmentTask(void const * argument);
void AmbulanceDepartmentTask(void const * argument);
void FireDepartmentTask(void const * argument);
void CoronaDepartmentTask(void const * argument);
uint32_t randNum(void);
uint32_t generateRandomNumber(uint32_t min, uint32_t max);
void PaddingStartOfFunc(void);

#define MAX_POLICE_CARS 3
#define MAX_FIRE_CARS 2
#define MAX_AMBULANCE_CARS 4
#define MAX_CORONA_CARS 4

#endif /* INC_FRT_H_ */

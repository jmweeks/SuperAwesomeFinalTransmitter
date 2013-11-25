/**
  ******************************************************************************
  * @file    project_filter.h
  * @author  Group 6
  * @version V1.0.0
  * @date    1-November-2013
  * @brief   This defines public functions for a simple Linear Moving Average Filter:
	*           - Insert value into buffer
	*           - Calculate average of all elements in buffer
	*          It also defines a Moving Average Filter struct
  */

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx.h"

/* Defines ------------------------------------------------------------------*/

#ifndef MAX_MOVING_AVERAGE_FILTER_SIZE																										//safety check on max moving average filter size
#define MAX_MOVING_AVERAGE_FILTER_SIZE 32																									//Size of moving average circular filter window, control for stack size limitation
#endif

/* Exported Types ---------------------------------------------------------*/

/** @defgroup Structs
  * @{
  */

#ifndef MOVING_AVERAGE_STRUCT																															//safety check on moving average struct
#define MOVING_AVERAGE_STRUCT																															//define moving average struct

struct Moving_Average {																																		//moving average filter struct components
	float moving_values[MAX_MOVING_AVERAGE_FILTER_SIZE];																		//all filters this long
	uint32_t index;																																					//index in filter
	float average;																																					//average of used filter
	uint32_t filter_size;																																		//how much of available filter we use
};

#endif

/**
  * @}
  */

/* Public Functions ---------------------------------------------------------*/

/** @defgroup Public_Functions
  * @{
  */ 

void init_moving_average(struct Moving_Average *moving_average, uint32_t size);
void insert_value(struct Moving_Average *moving_average, float new_value);
void calculate_average(struct Moving_Average *moving_average);

/**
  * @}
  */

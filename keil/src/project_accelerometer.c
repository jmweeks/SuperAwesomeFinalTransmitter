/**
  ******************************************************************************
  * @file    
  * @author  
  * @version V1.0.0
  * @date    8-November-2013
  * @brief   Provides a structure and functionality to implement tilt detection
  */

/* Includes ------------------------------------------------------------------*/

#include <math.h>
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "stm32f4_discovery_lis302dl.h"
#include "project_accelerometer.h"

/* Private Functions ---------------------------------------------------------*/

/** @defgroup Private_Functions
  * @{
  */

/**
  * @brief  Get raw accelerometer readings and calculate orientation (roll, pitch)
	* @param  *orientation: Pointer to an Orientation struct, to update
  * @retval None
  */

static void read_accelerometer(struct Orientation *orientation) {
	int32_t reading[3];
	
	LIS302DL_ReadACC(reading);																																										//Read LIS302DL output register, and calculate the acceleration (mg)
	
	orientation->rawx = reading[0];
	orientation->rawy = reading[1];
	orientation->rawz = reading[2];
	
	orientation->x = (orientation->rawx*ACC11 + orientation->rawy*ACC21 + orientation->rawz*ACC31) + ACC10;				//Calibrate X
	orientation->y = (orientation->rawx*ACC12 + orientation->rawy*ACC22 + orientation->rawz*ACC32) + ACC20;				//Calibrate Y
	orientation->z = (orientation->rawx*ACC13 + orientation->rawy*ACC23 + orientation->rawz*ACC33) + ACC30;				//Calibrate Z
	
	orientation->pitch = 180*atan(orientation->x/sqrt(pow(orientation->y,2)+pow(orientation->z,2)))/PI;						//Calculate pitch angle
	orientation->roll = 180*atan(orientation->y/sqrt(pow(orientation->x,2)+pow(orientation->z,2)))/PI;						//Calculate roll angle
	orientation->yaw = 0;																																													//Calculate yaw angle (always 0 with the STM32F4 Discovery's accelerometer configuration)
}

/**
  * @}
  */

/**
  * @brief  Updates intensities of all LEDs simultaneously
	* @param  led_intensities[]: Array of intensity values to write to LEDs
	* @param  length: Length of led_intensities array
	* @param  TIMx: TIM to write CCR values to
  * @retval None
  */

void update_led_intensities(uint32_t led_intensities[], uint32_t length, TIM_TypeDef* TIMx) {
	if (length == 4)
	{
		TIM_SetCompare1(TIMx, led_intensities[0]);									//set the 4 TIM4 capture compare register values
		TIM_SetCompare2(TIMx, led_intensities[1]);									//based off LED intensity
		TIM_SetCompare3(TIMx, led_intensities[2]);									//
		TIM_SetCompare4(TIMx, led_intensities[3]);									//
	}
}

/**
  * @}
  */

/* Public Functions ---------------------------------------------------------*/

/** @defgroup Public_Functions
  * @{
  */

/**
  * @brief  Get accelerometer's current reading, and update Orientation's moving average struct
	* @param  *orientation: Pointer to an Orientation struct, to update
  * @retval None
  */

void update_orientation(struct Orientation *orientation) {
	read_accelerometer(orientation);																																							//Read data into orientation struct
	
	insert_value(&orientation->moving_average_pitch, orientation->pitch);																					//Insert most recent pitch reading into moving average filter
	insert_value(&orientation->moving_average_roll, orientation->roll);																						//Insert most recent roll reading into moving average filter

	calculate_average(&orientation->moving_average_pitch);																												//Calculate pitch average
	calculate_average(&orientation->moving_average_roll);																													//Calculate roll average
}

/**
  * @brief  Initalize orientation struct
	* @param  *orientation: Pointer to Orientation structure to be initialized
  * @retval None
  */

void init_orientation(struct Orientation *orientation, osThreadId **tid_thread_orientation) {
	orientation->rawx = 0;																																												//Set ALL THE THINGS to zero
	orientation->rawy = 0;
	orientation->rawz = 0;
	orientation->x = 0;
	orientation->y = 0;
	orientation->z = 0;
	orientation->pitch = 0;
	orientation->roll = 0;
	orientation->yaw = 0;
	init_moving_average(&orientation->moving_average_pitch, ACCELEROMETER_MOVING_AVERAGE_FILTER_SIZE);						//Initialize moving average filter for pitch reading
	init_moving_average(&orientation->moving_average_roll, ACCELEROMETER_MOVING_AVERAGE_FILTER_SIZE);							//Initialize moving average filter for roll reading
	init_moving_average(&orientation->moving_average_yaw, ACCELEROMETER_MOVING_AVERAGE_FILTER_SIZE);							//Initialize moving average filter for yaw reading
	
	osThreadDef(orientationThread, osPriorityNormal, 1, 0);
	orientation->threadID = osThreadCreate(osThread(orientationThread), orientation);
	
	osMutexDef(orientationMutex);
	orientation->mutexID=osMutexCreate(osMutex(orientationMutex));
	
	*tid_thread_orientation = &(orientation->threadID);
}

/**
  * @brief  Reads accelerometer to get orientation and updates LEDs to display it
	* @param  *orientation: pointer to an Orientation struct representing the accelerometer
  * @retval None
  */

void display_orientation(struct Orientation *orientation) {
	update_orientation(orientation);																																													//Update Orientation with new accelerometer values
	//printf("Pitch: %fd\tRoll: %fd\n", orientation->moving_average_pitch.average, orientation->moving_average_roll.average); 	//Print new orientation to debugger stream
	uint32_t new_led_intensities[4];
	if (orientation->moving_average_roll.average >= 0) {
		new_led_intensities[2] = orientation->moving_average_roll.average * ACCELEROMETER_PWM_STEPS / 90;														//Intensity based off of ratio of angle to 90d
		new_led_intensities[0] = 0;																																														//Also ensures that opposing LEDs are not on at the same time
	} else {
		new_led_intensities[2] = 0;
		new_led_intensities[0] = -orientation->moving_average_roll.average * ACCELEROMETER_PWM_STEPS / 90;													//Set LED for -roll, turn +roll LED off
	}
	if (orientation->moving_average_pitch.average >= 0) {
		new_led_intensities[3] = orientation->moving_average_pitch.average * ACCELEROMETER_PWM_STEPS / 90;													//set +pitch LED on to specified intensity, turn off -pitch LED
		new_led_intensities[1] = 0;
	} else {
		new_led_intensities[3] = 0;
		new_led_intensities[1] = -orientation->moving_average_pitch.average * ACCELEROMETER_PWM_STEPS / 90;													//set -pitch LED to specified intensity, turn off +pitch LED
	}
	update_led_intensities(new_led_intensities, sizeof(new_led_intensities)/sizeof(new_led_intensities[0]), TIM4);								//Update LED intensities
}

void orientationThread (void const *argument) {
	struct Orientation *orientation;
	orientation = (struct Orientation *)argument;
	
	while(1) {
		osSignalWait(0x0001, osWaitForever);
		osMutexWait(orientation->mutexID, osWaitForever);
		update_orientation(orientation);
		osMutexRelease(orientation->mutexID);
	}
}

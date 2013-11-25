/**
  ******************************************************************************
  * @file    
  * @author  \
  * @version V1.0.0
  * @date    1-November-2013
  * @brief   Provides a structure and functionality to implement tilt detection
  */

/* Includes ------------------------------------------------------------------*/


#include "stm32f4xx.h"
#include "project_filter.h"
#include "project_init.h"

#ifndef PI																												//Pi safety
#define PI 3.14159																								//It's Pi...
#endif

#define ACCELEROMETER_PWM_STEPS 200
#define ACCELEROMETER_PWM_FREQUENCY 500

#define ACCELEROMETER_MOVING_AVERAGE_FILTER_SIZE 10

#if ACCELEROMETER_MOVING_AVERAGE_FILTER_SIZE > MAX_MOVING_AVERAGE_FILTER_SIZE					//safety check on filter size, can overflow stack and freeze system
#undef ACCELEROMETER_MOVING_AVERAGE_FILTER_SIZE																				//if we're over limit, undefine filter size
#define ACCELEROMETER_MOVING_AVERAGE_FILTER_SIZE MAX_MOVING_AVERAGE_FILTER_SIZE				//set to max allowable filter size
#endif


/* Defines ------------------------------------------------------------------*/


//Calibration Matrix
#define ACC11 1.0177																							//Calibrating X, X scale factor
#define ACC12 0.0202																							//Calibrating X, Y scale factor
#define ACC13 0.0003																							//Calibrating X, Z scale factor

#define ACC21 0.0076																							//Calibrating Y, X scale factor
#define ACC22 1.0109																							//Calibrating Y, Y scale factor
#define ACC23 0.0030																							//Calibrating Y, Z scale factor

#define ACC31 0.0052																							//Calibrating Z, X scale factor
#define ACC32 -0.0120																							//Calibrating Z, Y scale factor
#define ACC33 0.9686																							//Calibrating Z, Z scale factor

//Calibration Offsets
#define ACC10 18.5581																							//Calibrating X, X offset
#define ACC20 21.9931																							//Calibrating Y, Y offset
#define ACC30 86.8937																							//Calibrating Z, Z offset


/* Exported Types ---------------------------------------------------------*/

/** @defgroup Structs
  * @{
  */


struct Orientation {
	int32_t rawx;																										//raw x sample from accelerometer
	int32_t rawy;																										//raw y sample from accelerometer
	int32_t rawz;																										//raw z sample from accelerometer
	int32_t x;																											//calibrated x value
	int32_t y;																											//calibrated y value
	int32_t z;																											//calibrated z value
	float roll;																											//roll angle
	float pitch;																										//pitch angle
	float yaw;																											//yaw angle
	struct Moving_Average moving_average_roll;											//average roll angle struct
	struct Moving_Average moving_average_pitch;											//average pitch angle struct
	struct Moving_Average moving_average_yaw;												//average yaw angle struct
};

/**
  * @}
  */

/* Public Functions ---------------------------------------------------------*/

/** @defgroup Public_Functions
  * @{
  */


void display_orientation(struct Orientation *orientation);				//
void update_orientation(struct Orientation *orientation);					//
void init_orientation(struct Orientation *orientation);						//
void init_accelerometer(void);																		//

/**
  * @}
  */

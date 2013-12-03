/*!
	@file project_init.h
	Header file that supports project_init.c source file.
*/
#include "stm32f4xx.h"
#include "stm32f4_discovery_lis302dl.h"

#define TIM5_FREQUENCY 15
#define TIM3_FREQUENCY 3
#define TIM4_FREQUENCY 1
#define TIM2_FREQUENCY 25

#define PUSHBUTTON_GPIO GPIOA
#define PUSHBUTTON_PIN GPIO_Pin_0

#define Y_MIN				0
#define Y_MAX				6
#define Z_MIN				0
#define Z_MAX				1
#define	ANGLE_MIN		0
#define ANGLE_MAX		13

#define ORIENTATION_MIN -90
#define ORIENTATION_MAX 90

#define STARTING_Y 0
#define STARTING_Z 0
#define STARTING_ANGLE 0
#define STARTING_MAGNET 0
#define NO_UPDATE -1

void init_userButton(void);
void init_TIM5(void);
void init_TIM3(void);
void init_TIM4(void);
void init_TIM2(void);
void init_accelerometer(void);

#include "stm32f4xx.h"

#define PUSHBUTTON_DELAY 25
#define TIM5_FREQUENCY 25
#define TIM3_FREQUENCY 6

#define Y_MIN				0
#define Y_MAX				6
#define Z_MIN				0
#define Z_MAX				1
#define	ANGLE_MIN		0
#define ANGLE_MAX		13

#define ORIENTATION_MIN -90
#define ORIENTATION_MAX 90

void init_pushbutton(void);
void init_TIM5(void);
void init_TIM3(void);

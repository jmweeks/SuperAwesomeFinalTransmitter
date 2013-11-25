#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_init.h"
#include "project_pwm.h"
#include "project_servo.h"
#include "project_filter.h"
#include "project_accelerometer.h"
#include "project_lcd.h"
#include "project_magnet.h"
#include "project_robot.h"
#include "CC2500.h"


/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void thread(void const *argument);

static struct Servo boomServo, crowdServo, swingServo;
static struct Robot robot;
static struct Magnet magnet;

//! Thread structure for above thread
osThreadDef(thread, osPriorityNormal, 1, 0);

uint8_t  dummyData[2] ={0,0};
uint8_t dummyAddr;
uint8_t dummyWrite;
uint8_t state;
uint8_t buffer_space;
uint8_t dummyArray[64];
uint8_t testArray[3] = {1,1,1};


/*!
 @brief Program entry point
 */
int main (void) {
	osThreadId tid_thread;
	CC2500_Init();
	init_TIM4(1 / SERVO_DUTY_CYCLE_STEP, SERVO_FREQUENCY);
	init_LEDS_PWM();
	init_servo(&boomServo, 1, BOOM_ANGLE_MAX, BOOM_ANGLE_MIN);
	init_servo(&crowdServo, 2, CROWD_ANGLE_MAX, CROWD_ANGLE_MIN);
	init_servo(&swingServo, 3, SWING_ANGLE_MAX, SWING_ANGLE_MIN);
	init_robot(&robot, &boomServo, &crowdServo, &swingServo);
	init_magnet(&magnet, RCC_AHB1Periph_GPIOD, GPIOD, GPIO_Pin_15);
	
	tid_thread = osThreadCreate(osThread(thread), NULL);
	
	CC2500_config_transmitter();
	
	dummyAddr = 0x3F;
// 	CC2500_StrobeSend(SRES_T,&state,&buffer_space);
// 	osDelay(1000);
	
//	CC2500_StrobeSend(SFTX_T,&state,&buffer_space);
//	osDelay(1000);
//	
//	CC2500_StrobeSend(SIDLE_T,&state,&buffer_space);
//	osDelay(1000);
//	
//	CC2500_StrobeSend(SIDLE_T,&state,&buffer_space);
//	osDelay(1000);
//	
//	CC2500_StrobeSend(SCAL_T,&state,&buffer_space);
//	osDelay(1000);
/////////////////////////
	
	uint8_t data[4] = {0,0,0,0};
//	
//	Wireless_TX(data);

	CC2500_StrobeSend(SIDLE_R,&state,&buffer_space);
	osDelay(1000);
	
	CC2500_StrobeSend(SFRX_R,&state,&buffer_space);
	osDelay(1000);
	
	
	CC2500_StrobeSend(SIDLE_R,&state,&buffer_space);
	osDelay(1000);
	
	CC2500_StrobeSend(SCAL_R,&state,&buffer_space);
	osDelay(1000);

	CC2500_StrobeSend(SRX_R,&state,&buffer_space);
	osDelay(1000);

	Wireless_RX(data);
	osDelay(osWaitForever);
}

void thread (void const *argument) {
	parkRobot(&robot);
	osDelay(1000);
	while(1) {
		moveRobot(&robot, 0, 0, 11);
		turnMagnetOff(&magnet);
		osDelay(2000);
		moveRobot(&robot, 1, 0, 11);
		turnMagnetOff(&magnet);
		osDelay(2000);
		moveRobot(&robot, 2, 0, 11);
		turnMagnetOff(&magnet);
		osDelay(2000);
		moveRobot(&robot, 3, 0, 11);
		turnMagnetOff(&magnet);
		osDelay(2000);
		moveRobot(&robot, 4, 0, 11);
		turnMagnetOff(&magnet);
		osDelay(2000);
		moveRobot(&robot, 5, 0, 11);
		turnMagnetOff(&magnet);
		osDelay(2000);
		moveRobot(&robot, 6, 0, 11);
		turnMagnetOn(&magnet);
		osDelay(2000);
	}
}

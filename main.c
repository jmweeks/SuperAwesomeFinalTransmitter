#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_keypad.h"
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
void thread_keypad(void const * arguments);

//! Thread structure for above thread
osThreadDef(thread, osPriorityNormal, 1, 0);
osThreadDef(thread_keypad, osPriorityNormal, 1, 0);

osThreadId tid_thread_keypad;
	
//Mutex for keypad hold
static uint32_t keypad_input;			//
osMutexDef(keypad_mutex);
static osMutexId keypad_mutex;

//Message Pool for sending
osPoolDef(message_pool, 16, keypad_input);				//sender
osPoolId message_pool;														//sender

//uint8_t  dummyData[2] ={0,0};
//uint8_t dummyAddr;
//uint8_t dummyWrite;
uint8_t state;
uint8_t buffer_space;
//uint8_t dummyArray[64];
//uint8_t testArray[3] = {1,1,1};


/*!
 @brief Program entry point
 */
int main (void) {
	osThreadId tid_thread;

	

	osMutexCreate(osMutex(keypad_mutex));
	
	CC2500_Init();
	init_TIM4(1 / SERVO_DUTY_CYCLE_STEP, SERVO_FREQUENCY);
	init_LEDS_PWM();
	init_keypad_row_output();
	init_TIM5();
	
	tid_thread = osThreadCreate(osThread(thread), NULL);
	tid_thread_keypad = osThreadCreate(osThread(thread_keypad),NULL);
	
	CC2500_config_transmitter();
	
	goToTX();

	osDelay(osWaitForever);
}

void thread (void const *argument) {
	uint8_t data[4];
	while(1) {
		
	}
}

void thread_keypad (void const *arguments) {
	struct Keypad keypad;
	init_keypad(&keypad);
	
	while(1) {
		osSignalWait(0x0001, osWaitForever);
		osMutexWait(keypad_mutex, osWaitForever);
		get_keypad_entry(&keypad);
		if (keypad.key_char!=0xFF){
			keypad_input=(keypad.key_char);
		}
		osMutexRelease(keypad_mutex);
		
		osDelay(10);
		osSignalClear(tid_thread_keypad, 0x0001);				//clear signal, safety
	}
}

/**
  * @brief  IRQ handler for TIM5
	* @param  None
  * @retval None
  */

void TIM5_IRQHandler(void) {
  if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)			//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);						//Reset interrupt pending bit
		osSignalSet(tid_thread_keypad, 0x0001);								//send signal to keypad thread
  }
}

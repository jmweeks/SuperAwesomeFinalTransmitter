#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_keypad.h"
#include "project_init.h"
#include "project_filter.h"
#include "project_accelerometer.h"
#include "project_lcd.h"
#include "CC2500.h"

/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void thread(void const *argument);

static struct Keypad keypad;
static struct Lcd lcd;
static struct Orientation orientation;

//! Thread structure for above thread
osThreadDef(thread, osPriorityNormal, 1, 0);

static osThreadId *tid_thread_keypad;
static osThreadId *tid_thread_orientation;
static osThreadId tid_thread;

static uint32_t mode = 0;
static uint32_t pushbutton_pressed = 0;

uint8_t data_testing[64];
uint8_t state;
uint8_t buffer_space;

/*!
 @brief Program entry point
 */
int main (void) {
	
	CC2500_Init();
	init_keypad(&keypad, &tid_thread_keypad);
	init_lcd(&lcd);
	init_orientation(&orientation, &tid_thread_orientation);
	init_pushbutton();
	CC2500_config_transmitter();
	
	uint8_t data[4];
	
	goToRX(&state, &buffer_space);
	
	tid_thread = osThreadCreate(osThread(thread), NULL);

	osDelay(osWaitForever);
}

void thread (void const *argument) {
	uint8_t data[4];
	uint8_t roll;
	while(1) {
		wireless_RX(data, 4, &state, &buffer_space);
		//Wireless_RX(data);
//		Wireless_TX(data);
//		if (pushbutton_pressed) {
//			if (mode) {
//				mode = 0;
//			} else {
//				mode = 1;
//			}
//			osDelay(PUSHBUTTON_DELAY);																				//delay 10ms (debounce)
//			while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {	//reading pushbutton input
//				osDelay(PUSHBUTTON_DELAY);																			//delay 10ms (debounce)
//			}
//			pushbutton_pressed = 0;
//		}
//		if (mode) {
//			osMutexWait(keypad.mutexID, osWaitForever);
//			roll = keypad.key_char;
//			osMutexRelease(keypad.mutexID);
//			if (roll != 0xFF) {
//				roll+=48;
//				writeToLcd(&lcd, (char*)(&roll), 1);
//			}
//		} else {
//			osMutexWait(orientation.mutexID, osWaitForever);
//			roll = orientation.moving_average_pitch.average/9+48;
//			osMutexRelease(orientation.mutexID);
//			writeToLcd(&lcd, (char *)&roll, 1);
//		}
		osDelay(1000);
	}
}

void TIM2_IRQHandler(void) {
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)			//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);						//Reset interrupt pending bit
		osSignalSet(*tid_thread_orientation, 0x0001);								//send signal to keypad thread
  }
}

void TIM5_IRQHandler(void) {
  if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)			//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);						//Reset interrupt pending bit
		osSignalSet(*tid_thread_keypad, 0x0001);								//send signal to keypad thread
  }
}

/**
  * @brief  IRQ handler for EXTI0
	* @param  None
  * @retval None
  */

void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)								//Checks interrupt status register to ensure an interrupt is pending
  {
    EXTI_ClearITPendingBit(EXTI_Line0);										//Reset interrupt pending bit
		pushbutton_pressed++;
  }
}

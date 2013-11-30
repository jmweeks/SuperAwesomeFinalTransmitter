#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_keypad.h"
#include "project_init.h"
#include "project_filter.h"
#include "project_accelerometer.h"
#include "project_lcd.h"
#include "CC2500.h"
#include "project_transmitter.h"

/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void thread(void const *argument);

static struct Keypad keypad;
static struct Lcd lcd;
static struct Orientation orientation;
static struct Transmitter transmitter;

//! Thread structure for above thread
osThreadDef(thread, osPriorityNormal, 1, 0);

static osThreadId *tid_thread_keypad;
static osThreadId *tid_thread_orientation;
static osThreadId *tid_thread_transmitter;
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
	
	//CC2500_Init();
	init_keypad(&keypad, &tid_thread_keypad);
	init_lcd(&lcd);
	init_orientation(&orientation, &tid_thread_orientation);
	init_pushbutton();
	init_transmitter(&transmitter, &tid_thread_transmitter);
	//CC2500_config_transmitter();
	
	//goToTX(&state, &buffer_space);
	
	tid_thread = osThreadCreate(osThread(thread), NULL);

	osDelay(osWaitForever);
}

void thread (void const *argument) {
	uint8_t data[4];
	uint8_t y, z, angle, magnet;
	uint8_t tempY, tempZ, tempAngle;
	char yAscii[2], zAscii[2], angleAscii[2], magnetAscii[3];
	uint16_t currentKey, previousKey;
	uint32_t i = 0;
	while(1) {
		
		if (pushbutton_pressed) {
			if (mode) {
				mode = 0;
				clearCursor(&lcd);
			} else {
				mode = 1;
			}

			osDelay(PUSHBUTTON_DELAY);																				//delay 10ms (debounce)
			while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {	//reading pushbutton input
				osDelay(PUSHBUTTON_DELAY);																			//delay 10ms (debounce)
			}
			pushbutton_pressed = 0;
		}
		
		osMutexWait(keypad.mutexID, osWaitForever);
		if (keypad.new_data_available) {
			currentKey = keypad.key_char;
			keypad.new_data_available = 0;
		} else {
			currentKey = 0xFF;
		}
		osMutexRelease(keypad.mutexID);
		
		if (currentKey == 0x0A) {
			if (magnet) {
				magnet = 0;
			} else {
				magnet = 1;
			}
		}
		
		if (mode) {
			if (currentKey < 0x0A) {
				switch (i) {
					case 0:
						tempY = currentKey;
						break;
					case 1:
						tempY = tempY*10+currentKey;
						if (tempY > Y_MAX) {
							y = Y_MAX;
						} else {
							y = tempY;
						}
						break;
					case 2:
						tempZ = currentKey;
						break;
					case 3:
						tempZ = tempZ*10+currentKey;
						if (tempZ > Z_MAX) {
							z = Z_MAX;
						} else {
							z = tempZ;
						}
						break;
					case 4:
						tempAngle = currentKey;
						break;
					default:
						tempAngle = tempAngle*10+currentKey;
						if (tempAngle > ANGLE_MAX) {
							angle = ANGLE_MAX;
						} else {
							angle = tempAngle;
						}
						break;
				}
				if (i<5) {
					i++;
				} else {
					i = 0;
				}
			}
			data[0] = y;
			data[1] = z;
			data[2] = angle;
			data[3] = magnet;
			if (currentKey == 0x0B) {
				osMutexWait(transmitter.mutexID, osWaitForever);
				uint32_t c;
				for (c=0; c<sizeof(transmitter.data)/sizeof(transmitter.data[0]); c++) {
					transmitter.data[c] = data[c];
				}
				osMutexRelease(transmitter.mutexID);
			}
			setCursor(&lcd, 3*i/2+6);
		} else {
			osMutexWait(orientation.mutexID, osWaitForever);
			y = 3 * (Y_MAX + 1) * (orientation.moving_average_pitch.average - ORIENTATION_MIN) / (ORIENTATION_MAX - ORIENTATION_MIN) - (Y_MAX + 1);
			angle = 3 * (ANGLE_MAX + 1) * (-orientation.moving_average_roll.average - ORIENTATION_MIN) / (ORIENTATION_MAX - ORIENTATION_MIN) - (ANGLE_MAX + 1);
			if (y < Y_MIN) {
				y = Y_MIN;
			} else if (y > Y_MAX) {
				y = Y_MAX;
			}
			if (angle < ANGLE_MIN) {
				angle = ANGLE_MIN;
			} else if (angle > ANGLE_MAX) {
				angle = ANGLE_MAX;
			}
			osMutexRelease(orientation.mutexID);
			if (currentKey == 0x0B) {
				if (z) {
					z = 0;
				} else {
					z = 1;
				}
			}
			if (z) {
				data[0] = y;
			}
			data[1] = z;
			if (z) {
					data[2] = angle;
			}
			data[3] = magnet;
			osMutexWait(transmitter.mutexID, osWaitForever);
			uint32_t c;
			for (c=0; c<sizeof(transmitter.data)/sizeof(transmitter.data[0]); c++) {
				transmitter.data[c] = data[c];
			}
			osMutexRelease(transmitter.mutexID);
		}
			
		updateLcdData(&lcd, y, z, angle, magnet, mode);
		osDelay(100);
	}
}

void TIM2_IRQHandler(void) {
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)			//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);						//Reset interrupt pending bit
		osSignalSet(*tid_thread_orientation, 0x0001);								//send signal to keypad thread
  }
}

void TIM3_IRQHandler(void) {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)			//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);						//Reset interrupt pending bit
		osSignalSet(*tid_thread_transmitter, 0x0001);								//send signal to keypad thread
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

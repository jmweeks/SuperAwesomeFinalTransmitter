/*!
	@file main.c
	Main file of transmitter program. 
*/
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
#include "project_timeLeft.h"
#include "project_pushbutton.h"

/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 @retval None
 */
void controllerThread(void const *argument);

void updateTransmitterData(int8_t y, int8_t z, int8_t angle, int8_t magnet, int8_t time);

static struct Keypad keypad;
static struct Lcd lcd;
static struct Orientation orientation;
static struct Transmitter transmitter;
static struct TimeLeft timeLeft;
static struct Pushbutton pushbutton;

//! Thread structure for above thread
osThreadDef(controllerThread, osPriorityNormal, 1, 0);

static osThreadId *tid_thread_keypad;
static osThreadId *tid_thread_orientation;
static osThreadId *tid_thread_transmitter;
static osThreadId *tid_thread_timeLeft;
static osThreadId *tid_thread_pushbutton;
static osThreadId tid_controllerThread;

static uint32_t mode = 0;
static uint32_t pushbutton_pressed = 0;

uint8_t data_testing[64];
uint8_t state;
uint8_t buffer_space;

/*!
 @brief Program entry point
 */
int main (void) {
	struct LcdInit lcdInit;
	struct KeypadInit keypadInit;
	
	lcdInit.GPIO = GPIOD;
	lcdInit.periph = RCC_AHB1Periph_GPIOD;
	lcdInit.pinRS = GPIO_Pin_8;
	lcdInit.pinRW = GPIO_Pin_9;
	lcdInit.pinE = GPIO_Pin_10;
	lcdInit.pinD4 = GPIO_Pin_11;
	lcdInit.pinD5 = GPIO_Pin_12;
	lcdInit.pinD6 = GPIO_Pin_13;
	lcdInit.pinD7 = GPIO_Pin_14;
	
	keypadInit.GPIO = GPIOD;
	keypadInit.periph = RCC_AHB1Periph_GPIOD;
	keypadInit.rowPins = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	keypadInit.colPins = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	
	init_TIM5(); //Keypad
	init_TIM2(); //Accelerometer
	init_TIM3(); //Transmitter
	init_TIM4(); //TimeLeft
	init_accelerometer();
	init_userButton();
	
	init_keypad(&keypad, &tid_thread_keypad, &keypadInit);
	init_lcd(&lcd, &lcdInit);
	init_orientation(&orientation, &tid_thread_orientation);
	init_transmitter(&transmitter, &tid_thread_transmitter);
	init_timeLeft(&timeLeft, &tid_thread_timeLeft);
	init_pushbutton(&pushbutton, &tid_thread_pushbutton);
	
	tid_controllerThread = osThreadCreate(osThread(controllerThread), NULL);

	osDelay(osWaitForever);
}

/*!
 @brief Main thread of transmitter. 
 @param Unused
 @retval None
 */
void controllerThread (void const *argument) {
	uint8_t y, z, angle, magnet;
	uint8_t tempY, tempZ, tempAngle;
	uint16_t currentKey;
	uint32_t i = 0;
	uint32_t time = 0;
	uint32_t initialTime = 0;
	uint32_t isStarted = 0;

	osMutexWait(timeLeft.mutexID, osWaitForever);
	initialTime = timeLeft.initialTime;
	osMutexRelease(timeLeft.mutexID);

	while(1) {
		osMutexWait(pushbutton.mutexID, osWaitForever);
		if (pushbutton.buttonPressed) {
			pushbutton_pressed = 1;
			pushbutton.buttonPressed = 0;
		} else {
			pushbutton_pressed = 0;
		}
		osMutexRelease(pushbutton.mutexID);
		
		if (pushbutton_pressed) {
			if (mode) {
				mode = 0;
				isStarted = 0;
				y = STARTING_Y;
				z = STARTING_Z;
				angle = STARTING_ANGLE;
				magnet = STARTING_MAGNET;
				clearCursor(&lcd);
				osMutexWait(timeLeft.mutexID, osWaitForever);
				resetTimeLeft(&timeLeft);
				osMutexRelease(timeLeft.mutexID);
			} else {
				mode = 1;
			}
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
				if (mode || !z) {
					magnet = 1;
				}
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
			
			updateTransmitterData(y, z, angle, magnet, 0);
			
			setCursor(&lcd, 3*i/2+6);
		} else {
			if (currentKey == 0x0B) {
				isStarted = 1;
			}
			
			osMutexWait(timeLeft.mutexID, osWaitForever);
			time = timeLeft.timeLeft;
			osMutexRelease(timeLeft.mutexID);
			
			if (!time) {
				isStarted = 0;
				y = STARTING_Y;
				z = STARTING_Z;
				angle = STARTING_ANGLE;
				magnet = STARTING_MAGNET;
				updateTransmitterData(y, z, angle, magnet, 0);
				resetTimeLeft(&timeLeft);
			}
				
			if (!isStarted) {
				osMutexWait(timeLeft.mutexID, osWaitForever);
				timeLeft.started = 0;
				osMutexRelease(timeLeft.mutexID);
				
				updateTransmitterData(y, z, angle, magnet, 0);
			} else {
				osMutexWait(timeLeft.mutexID, osWaitForever);
				timeLeft.started = 1;
				osMutexRelease(timeLeft.mutexID);
				
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
						if (!magnet) {
							z = 0;
						}
					} else {
						z = 1;
					}
				}
				if (z) {
					updateTransmitterData(y, z, angle, magnet, time*15/initialTime+1);
				} else {
					updateTransmitterData(NO_UPDATE, z, NO_UPDATE, magnet, time*15/initialTime+1);
				}
			}
		}
			
		updateLcdData(&lcd, y, z, angle, magnet, mode, time);
		osDelay(100);
	}
}

/*!
 @brief Updates data that the transmitter will send to receier. 
 @param y: y-coordinate (out)
 @param z: z-coordinate (up/down). Binary value.
 @param angle: angle position of arm.
 @param magnet: magnet state: on/off.
 @param time: Time left until game is over.
 @retval None
 */
void updateTransmitterData(int8_t y, int8_t z, int8_t angle, int8_t magnet, int8_t time) {
	osMutexWait(transmitter.mutexID, osWaitForever);
	if (y>=0) {
		transmitter.data[0] = y;
	}
	if (z>=0) {
		transmitter.data[1] = z;
	}
	if (angle>=0) {
		transmitter.data[2] = angle;
	}
	if (magnet>=0) {
		transmitter.data[3] = magnet;
	}
	if (time>=0) {
		transmitter.data[4] = time;
	}
	osMutexRelease(transmitter.mutexID);
}

/**
  * @brief  IRQ handler for TIM2.
	* @param  None
  * @retval None
  */
void TIM2_IRQHandler(void) {
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)			//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);						//Reset interrupt pending bit
		osSignalSet(*tid_thread_orientation, 0x0001);								//send signal to keypad thread
  }
}

/**
  * @brief  IRQ handler for TIM3.
	* @param  None
  * @retval None
  */
void TIM3_IRQHandler(void) {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)			//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);						//Reset interrupt pending bit
		osSignalSet(*tid_thread_transmitter, 0x0001);								//send signal to keypad thread
  }
}

/**
  * @brief  IRQ handler for TIM5.
	* @param  None
  * @retval None
  */
void TIM5_IRQHandler(void) {
  if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)			//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);						//Reset interrupt pending bit
		osSignalSet(*tid_thread_keypad, 0x0001);								//send signal to keypad thread
  }
}

/**
  * @brief  IRQ handler for TIM4.
	* @param  None
  * @retval None
  */
void TIM4_IRQHandler(void) {
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)			//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);						//Reset interrupt pending bit
		osSignalSet(*tid_thread_timeLeft, 0x0001);								//send signal to keypad thread
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
		osSignalSet(*tid_thread_pushbutton, 0x0001);
  }
}

/*!
	@file project_keypad.c
	Contains functions for the keypad initialization and functionality.
*/
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_keypad.h"
#include "project_init.h"

/*!
 @brief Function that initializes the row of the keypad. 
 @param *keypad: Pointer to a Keypad structure.
 @retval None
 */
static void init_keypad_row_output(struct Keypad *keypad) {
	GPIO_InitTypeDef GPIO_InitStructure;																														//Initialize nested vector interrupt controller structure
	
  RCC_AHB1PeriphClockCmd(keypad->periph, ENABLE);																			//Enable clock to GPIOD
  
  GPIO_InitStructure.GPIO_Pin = keypad->colPins;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																							//Aternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																					//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																						//Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;																					//No pull - output
  GPIO_Init(keypad->GPIO, &GPIO_InitStructure);
	  
  GPIO_InitStructure.GPIO_Pin = keypad->rowPins;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;																					//Aternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																			//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																				//Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;																					//No pull - output
  GPIO_Init(keypad->GPIO, &GPIO_InitStructure);
	
	GPIO_SetBits(keypad->GPIO, keypad->colPins);
}

/*!
 @brief Function that initializes the column of the keypad. 
 @param *keypad: Pointer to a Keypad structure.
 @retval None
 */
static void init_keypad_column_output(struct Keypad *keypad) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHB1PeriphClockCmd(keypad->periph, ENABLE);																	//Enable clock to GPIOD
  
  GPIO_InitStructure.GPIO_Pin = keypad->colPins;			//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;																					//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																			//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																				//
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;																					//
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	  
  GPIO_InitStructure.GPIO_Pin = keypad->rowPins;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																					//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																			//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																				//
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;																				//
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(keypad->GPIO, keypad->rowPins);
}


/*!
 @brief Function checks if a keypad button was pressed. 
 @param *keypad: Pointer to a Keypad structure.
 @retval None
 */
static void get_button_press(struct Keypad *keypad) {
	uint8_t row_press;										//
	uint8_t column_press;									//
	uint8_t test;
	
	init_keypad_row_output(keypad);							//reinitialize the rows to be outputs for next check
	
	row_press = GPIO_ReadInputData(GPIOD) & (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);								//get GPIOD data register
	keypad->row_output=row_press;					//set the struct row value
	
	init_keypad_column_output(keypad);					//reinitialize columns as outputs so we can find key press
	
	column_press = GPIO_ReadInputData(GPIOD) & (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);						//get GPIOD data register again
	keypad->column_output=column_press;		//set the struct column value
	
	test = column_press | row_press;
	
	keypad->key_press = test;
}

/*!
 @brief Function that initializes the keypad peripheral. 
 @param *keypad: Pointer to a Keypad structure.
 @param **tid_thread_keypad: Pointer to a thread control block.
 @param *keypadInit: pointer to a KeypadInit structure.
 @retval None
 */
void init_keypad(struct Keypad *keypad, osThreadId **tid_thread_keypad, struct KeypadInit *keypadInit) {
	keypad->row_output					=0x0;
	keypad->column_output				=0x0;
	keypad->key_press						=0x0;
	keypad->key_char						=0x0;
	keypad->new_data_available	=0x0;
	keypad->old_key_char				=0x0;
	keypad->fNumber							=0x0;
	keypad->fRequired = keypadInit->fRequired;
	
	keypad->GPIO = keypadInit->GPIO;
	keypad->periph = keypadInit->periph;
	keypad->rowPins = keypadInit->rowPins;
	keypad->colPins = keypadInit->colPins;
	
	osThreadDef(keypadThread, osPriorityNormal, 1, 0);
	keypad->threadID = osThreadCreate(osThread(keypadThread), keypad);
	
	osMutexDef(keypadMutex);
	keypad->mutexID=osMutexCreate(osMutex(keypadMutex));
	
	*tid_thread_keypad = &(keypad->threadID);
}


/*!
 @brief Function that decipers the hex value of the button pressed and translates it to a value for a button: 0-9, * or #.
 @param *keypad: Pointer to a Keypad structure. 
 @retval None
 */
static void get_keypad_entry(struct Keypad *keypad) {
	uint16_t key_input;
	uint32_t button_value;
	
	get_button_press(keypad);
	
	key_input=keypad->key_press;
	
	if(key_input==0x23) {
		button_value=0x1;
	}else if(key_input==0x63) {
		button_value=0x2;
	}else if(key_input==0xA3) {
		button_value=0x3;
	}else if(key_input==0x25) {
		button_value=0x4;
	}else if(key_input==0x65) {
		button_value=0x5;
	}else if(key_input==0xA5) {
		button_value=0x6;
	}else if(key_input==0x29) {
		button_value=0x7;
	}else if(key_input==0x69) {
		button_value=0x8;
	}else if(key_input==0xA9) {
		button_value=0x9;
	}else if(key_input==0x31) {
		button_value=0xA; //* (star) key got pressed
	}else if(key_input==0x71) {
		button_value=0x0;
	}else if(key_input==0xB1) {
		button_value=0xB; //# (pound) key got pressed
	} else {
		button_value=0xFF;
	}
	
	if (button_value != 0xFF) {
		if (keypad->old_key_char != button_value) {
			keypad->key_char = button_value;
			keypad->new_data_available = 1;
			keypad->old_key_char = button_value;
			keypad->fNumber = 0;
		}
	} else {
		keypad->fNumber++;
		if (keypad->fNumber == keypad->fRequired) {
			keypad->old_key_char = button_value;
		}
	}
}

/*!
 @brief Thread that gets that waits for a button to be pressed and gets the value of that key press. 
 @param *keypad: Pointer to a Keypad structure that gets passed to thread.
 @retval None
 */
void keypadThread (void const *argument) {
	struct Keypad *keypad;
	keypad = (struct Keypad *)argument;
	
	while(1) {
		osSignalWait(0x0001, osWaitForever);
		osMutexWait(keypad->mutexID, osWaitForever);
		get_keypad_entry(keypad);
		osMutexRelease(keypad->mutexID);
	}
}

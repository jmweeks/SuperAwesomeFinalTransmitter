#include "stm32f4xx.h"

#include "project_keypad.h"

void init_keypad_row_output() {
	GPIO_InitTypeDef GPIO_InitStructure;																														//Initialize nested vector interrupt controller structure
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);																			//Enable clock to GPIOD
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																							//Aternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																					//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																						//Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;																					//No pull - output
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;																					//Aternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																			//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																				//Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;																					//No pull - output
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void init_keypad_column_output() {
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);																	//Enable clock to GPIOD
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;			//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;																					//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																			//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																				//
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;																					//
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																					//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																			//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																				//
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;																				//
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void init_keypad(struct Keypad *keypad) {
	keypad->row_output		=0x0;
	keypad->column_output	=0x0;
	keypad->key_press			=0x0;
	keypad->key_char			=0x0;
}

void init_TIM5() {
  NVIC_InitTypeDef NVIC_InitStructure;																								//NVIC initialization struct
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;																			//Timer initialization struct
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 					//Enable clock to TIM10
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;								//No clock division
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//Counts up
	TIM_TimeBaseStructure.TIM_Period = 0x0AFF;										//Period set for largest dynamic range of standard timer frequencies to be used
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00FF;									//Set prescaler to determine frequency
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;						//Restart RCR count after counting down to this value
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);								//Initialize struct parameters to TIM5
	
	TIM_Cmd(TIM5, ENABLE);																				//Enable specified peripheral
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);										//Enable new interrupt state
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;								//Specify interrupt request channel to be used
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; 	//Indicates pre-emption priority, 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; 				//Subpriority value 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 							//Enable interrupt request channel specified earlier
  NVIC_Init(&NVIC_InitStructure);																//Initialize NVIC for TIM3 with struct params
}

void get_keypad_entry(struct Keypad *keypad) {
//	uint16_t row_bits;
//	uint16_t column_bits;
	uint16_t key_input;
	uint32_t button_value;
	
	get_button_press(keypad);
	
	key_input=keypad->key_press;
//	column_bits=keypad->column_output;
	
	if(key_input==0xCC) {
		button_value=0x1;
	}else if(key_input==0xAC) {
		button_value=0x2;
	}else if(key_input==0x6C) {
		button_value=0x3;
	}else if(key_input==0xCA) {
		button_value=0x4;
	}else if(key_input==0xAA) {
		button_value=0x5;
	}else if(key_input==0x6A) {
		button_value=0x6;
	}else if(key_input==0xC6) {
		button_value=0x7;
	}else if(key_input==0xA6) {
		button_value=0x8;
	}else if(key_input==0x66) {
		button_value=0x9;
	}else if(key_input==0xCE) {
		button_value=0xA;
	}else if(key_input==0xAE) {
		button_value=0x0;
	}else if(key_input==0x6E) {
		button_value=0xB;
	} else {
		button_value=0xFF;
	}
	
	keypad->key_char=button_value;
}


void get_button_press(struct Keypad *keypad) {
	uint8_t row_press;										//
	uint8_t column_press;									//
	uint8_t test;
	
	row_press = GPIOD->IDR;								//get GPIOD data register
	keypad->row_output=row_press;					//set the struct row value
	
	init_keypad_column_output();					//reinitialize columns as outputs so we can find key press
	
	column_press = GPIOD->IDR;						//get GPIOD data register again
	keypad->column_output=column_press;		//set the struct column value
	
	test = column_press | row_press;
	
	keypad->key_press = test;
	
	init_keypad_row_output();							//reinitialize the rows to be outputs for next check
}





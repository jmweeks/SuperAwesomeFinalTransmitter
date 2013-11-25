#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_lcd.h"

static void delayMicroseconds(uint32_t microseconds) {
	uint32_t counter = SystemCoreClock / (microseconds * 1000000);
	while (counter != 0) {
		counter--;
	}
}

static void init_read(struct Lcd *lcd) {
	GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = lcd->pinD4 | lcd->pinD5 | lcd->pinD6 | lcd->pinD7;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;																							//Aternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																					//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																						//Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;																					//No pull - output
  GPIO_Init(lcd->GPIO, &GPIO_InitStructure);																										//Pass struct and initialize
	
	lcd->inReadInitState = 1;
}

static void init_write(struct Lcd *lcd) {
	GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = lcd->pinD4 | lcd->pinD5 | lcd->pinD6 | lcd->pinD7;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																							//Aternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																					//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																						//Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;																					//No pull - output
  GPIO_Init(lcd->GPIO, &GPIO_InitStructure);																										//Pass struct and initialize
	
	lcd->inReadInitState = 0;
}

static void readControlRegister(struct Lcd *lcd) {
	if (!lcd->inReadInitState) {
		init_read(lcd);
	}
	
	GPIO_ResetBits(lcd->GPIO, lcd->pinRS);
	GPIO_SetBits(lcd->GPIO, lcd->pinRW);
	GPIO_SetBits(lcd->GPIO, lcd->pinE);
	delayMicroseconds(LCD_WAIT_TIME);
	lcd->state = (GPIO_ReadInputData(lcd->GPIO) & (lcd->pinD4 | lcd->pinD5 | lcd->pinD6 | lcd->pinD7)) / (lcd->pinD4 >> 4);
	GPIO_ResetBits(lcd->GPIO, lcd->pinE);
	
	delayMicroseconds(LCD_WAIT_TIME);
	GPIO_SetBits(lcd->GPIO, lcd->pinE);
	delayMicroseconds(LCD_WAIT_TIME);
	lcd->state = lcd->state | ((GPIO_ReadInputData(lcd->GPIO) & (lcd->pinD4 | lcd->pinD5 | lcd->pinD6 | lcd->pinD7)) / (lcd->pinD4));
	GPIO_ResetBits(lcd->GPIO, lcd->pinE);
	delayMicroseconds(LCD_WAIT_TIME);
	
	lcd->cursorPosition = lcd->state & 0x7F;
}

static void waitUntilNotBusy(struct Lcd *lcd) {
	readControlRegister(lcd);
	while((lcd->state & 0x80) == 0x80) {
		delayMicroseconds(LCD_WAIT_TIME);
		readControlRegister(lcd);
	}
}

static void writeControlRegister(struct Lcd *lcd, uint8_t data) {
	
	if (lcd->inReadInitState) {
		init_write(lcd);
	}
	
	GPIO_ResetBits(lcd->GPIO, lcd->pinRS);
	GPIO_ResetBits(lcd->GPIO, lcd->pinRW);
	GPIO_SetBits(lcd->GPIO, lcd->pinE);
	
	uint16_t previousPortState = GPIO_ReadInputData(lcd->GPIO) & ~(lcd->pinD4 | lcd->pinD5 | lcd->pinD6 | lcd->pinD7);
	GPIO_Write(lcd->GPIO, previousPortState | ((uint16_t)(data >> 4) * (lcd->pinD4)));
	GPIO_ResetBits(lcd->GPIO, lcd->pinE);
	delayMicroseconds(LCD_WAIT_TIME);
	
	GPIO_SetBits(lcd->GPIO, lcd->pinE);
	GPIO_Write(lcd->GPIO, previousPortState | ((uint16_t)(data & 0x0F) * (lcd->pinD4)));
	GPIO_ResetBits(lcd->GPIO, lcd->pinE);
	delayMicroseconds(LCD_WAIT_TIME);
	
	waitUntilNotBusy(lcd);
}

static void updatePosition(struct Lcd *lcd, uint32_t position) {
	lcd->cursorPosition = position;
	if (lcd->cursorPosition >= LCD_WIDTH && lcd->cursorPosition < LCD_SECOND_ROW_STARTING_ADDRESS) {
		lcd->cursorPosition = LCD_SECOND_ROW_STARTING_ADDRESS;
	} else if (lcd->cursorPosition >= LCD_SECOND_ROW_STARTING_ADDRESS + LCD_WIDTH - 1) {
		lcd->cursorPosition = 0;
	}
	writeControlRegister(lcd, 0x80 | lcd->cursorPosition);
}

static void writeDataRegister(struct Lcd *lcd, uint8_t data) {
	
	if (lcd->inReadInitState) {
		init_write(lcd);
	}
	
	GPIO_SetBits(lcd->GPIO, lcd->pinRS);
	GPIO_ResetBits(lcd->GPIO, lcd->pinRW);
	GPIO_SetBits(lcd->GPIO, lcd->pinE);
	
	uint16_t previousPortState = GPIO_ReadInputData(lcd->GPIO) & ~(lcd->pinD4 | lcd->pinD5 | lcd->pinD6 | lcd->pinD7);
	GPIO_Write(lcd->GPIO, previousPortState | ((uint16_t)(data >> 4) * (lcd->pinD4)));
	GPIO_ResetBits(lcd->GPIO, lcd->pinE);
	delayMicroseconds(LCD_WAIT_TIME);
	
	GPIO_SetBits(lcd->GPIO, lcd->pinE);
	GPIO_Write(lcd->GPIO, previousPortState | ((uint16_t)(data & 0x0F) * (lcd->pinD4)));
	GPIO_ResetBits(lcd->GPIO, lcd->pinE);
	delayMicroseconds(LCD_WAIT_TIME);
	
	waitUntilNotBusy(lcd);
	
	updatePosition(lcd, lcd->cursorPosition+1);
}

void init_lcd(struct Lcd *lcd) {
	
	//PD0: RS
	//PD1: R/W
	//PD2: E
	//PD3-6: D4-7
	
	lcd->GPIO = GPIOD;
	lcd->periph = RCC_AHB1Periph_GPIOD;
	lcd->pinRS = GPIO_Pin_8;
	lcd->pinRW = GPIO_Pin_9;
	lcd->pinE = GPIO_Pin_10;
	lcd->pinD4 = GPIO_Pin_11;
	lcd->pinD5 = GPIO_Pin_12;
	lcd->pinD6 = GPIO_Pin_13;
	lcd->pinD7 = GPIO_Pin_14;
	lcd->inReadInitState = 0;
	lcd->cursorPosition = 0;
	
	RCC_AHB1PeriphClockCmd(lcd->periph, ENABLE);																			//Enable clock to GPIOD
	
	GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = lcd->pinRS | lcd->pinRW | lcd->pinE | lcd->pinD4 | lcd->pinD5 | lcd->pinD6 | lcd->pinD7;								//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																							//Aternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																					//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																						//Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;																					//No pull - output
  GPIO_Init(lcd->GPIO, &GPIO_InitStructure);																										//Pass struct and initialize
	
	waitUntilNotBusy(lcd);
	
	GPIO_ResetBits(lcd->GPIO, lcd->pinRS);
	GPIO_ResetBits(lcd->GPIO, lcd->pinRW);
	GPIO_SetBits(lcd->GPIO, lcd->pinE);
	
	uint16_t previousPortState = GPIO_ReadInputData(lcd->GPIO) & ~(lcd->pinD4 | lcd->pinD5 | lcd->pinD6 | lcd->pinD7);
	GPIO_Write(lcd->GPIO, previousPortState | ((uint16_t)(0x20 >> 4) * (lcd->pinD4)));
	GPIO_ResetBits(lcd->GPIO, lcd->pinE);
	delayMicroseconds(LCD_WAIT_TIME);
	
	waitUntilNotBusy(lcd);
	
	writeControlRegister(lcd, 0x28);	//Set 4-bit mode
	writeControlRegister(lcd, 0x06);	//Set increment cursor
	writeControlRegister(lcd, 0x0C);	//Set display on
	writeControlRegister(lcd, 0x01);	//Clear display
}

void clearLcd(struct Lcd *lcd) {
	writeControlRegister(lcd, 0x01);
	lcd->cursorPosition = 0;
}

void writeToLcd(struct Lcd *lcd, char text[], uint32_t length) {
	uint32_t i;
	clearLcd(lcd);
	for (i=0; i<length; i++) {
		writeDataRegister(lcd, text[i]);
	}
}

void writeToLcdPosition(struct Lcd *lcd, char text[], uint32_t length, uint32_t position) {
	uint32_t i;
	if (lcd->cursorPosition != position) {
		updatePosition(lcd, position);
	}
	for (i=0; i<length; i++) {
		writeDataRegister(lcd, text[i]);
	}
}

/*!
	@file project_lcd.c
	Source file used for LCD initialization and functionality.
*/
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include <string.h>

#include "project_lcd.h"

/*!
 @brief Custom delay function that accepts delay values in microseconds.
 @param microseconds: amount of delay expressed as a uint64_t.
 @retval None
 */
static void delayMicroseconds(uint64_t microseconds) {
	uint32_t counter = (microseconds * SystemCoreClock) / 1000000;
	while (counter != 0) {
		counter--;
	}
}

/*!
 @brief Initializes the LCD and sets it to "Read" state.
 @param *lcd: pointer to Lcd structure.
 @retval None
 */
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

/*!
 @brief Initializes the LCD and sets it to "Write" state.
 @param *lcd: pointer to Lcd structure.
 @retval None
 */
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
//exe cmd or updates cursor position
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

/*!
 @brief Function that waits until LCD is not busy executing a command.
 @param *lcd: pointer to Lcd structure.
 @retval None
 */
static void waitUntilNotBusy(struct Lcd *lcd) {
	readControlRegister(lcd);
	delayMicroseconds(LCD_WAIT_TIME);
	while((lcd->state & 0x80) == 0x80) {
		readControlRegister(lcd);
		delayMicroseconds(LCD_WAIT_TIME);
	}
}

/*!
 @brief Function that writes to the LCD control register, which executes commands such as updating address register
				(select which segment to write to), clear LCD, turn cursor on or off, etc.
 @param *lcd: pointer to Lcd structure.
 @param data: data byte to write to LCD
 @retval None
 */
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

/*!
 @brief Function that updates the cursor position on the LCD.
 @param *lcd: pointer to Lcd structure.
 @param position: new position of cursor
 @retval None
 */
static void updatePosition(struct Lcd *lcd, uint8_t position) {
	readControlRegister(lcd);
	if (position >= LCD_WIDTH && position < LCD_SECOND_ROW_STARTING_ADDRESS) {
		writeControlRegister(lcd, 0x80 | LCD_SECOND_ROW_STARTING_ADDRESS);
	} else if (position >= LCD_SECOND_ROW_STARTING_ADDRESS + LCD_WIDTH) {
		writeControlRegister(lcd, 0x80 | 0x00);
	} else if (lcd->cursorPosition != position) {
		writeControlRegister(lcd, 0x80 | position);
	}
}

/*!
 @brief Function that writes to the LCD data (display) registers.
 @param *lcd: pointer to Lcd structure.
 @param data: data byte to display to LCD
 @retval None
 */
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
	
	if (lcd->cursorPosition >= LCD_WIDTH && lcd->cursorPosition < LCD_SECOND_ROW_STARTING_ADDRESS) {
		writeControlRegister(lcd, 0x80 | LCD_SECOND_ROW_STARTING_ADDRESS);
	} else if (lcd->cursorPosition >= LCD_SECOND_ROW_STARTING_ADDRESS + LCD_WIDTH) {
		writeControlRegister(lcd, 0x80 | 0x00);
	} 
	
	//updatePosition(lcd, lcd->cursorPosition);
}


/*!
 @brief converts an integer to its ascii representation
 @param number: Number to convert to ascii
 @param data[]: array which stores ascii representation of number
 @retval None
 */
static void intToAscii(char number, char data[]) {
	if (number < 10) {
		data[0] = '0';
		data[1] = number+48;
	} else {
		data[0] = number/10+48;
		data[1] = (number+50)%10+48;
	}
}

/*!
 @brief Initializes the LCD peripheral.
 @param *lcd: pointer to Lcd structure.
 @param *lcdInit: pointer to LcdInit structure.
 @retval None
 */
void init_lcd(struct Lcd *lcd, struct LcdInit *lcdInit) {
	lcd->GPIO = lcdInit->GPIO;
	lcd->periph = lcdInit->periph;
	lcd->pinRS = lcdInit->pinRS;
	lcd->pinRW = lcdInit->pinRW;
	lcd->pinE = lcdInit->pinE;
	lcd->pinD4 = lcdInit->pinD4;
	lcd->pinD5 = lcdInit->pinD5;
	lcd->pinD6 = lcdInit->pinD6;
	lcd->pinD7 = lcdInit->pinD7;
	lcd->inReadInitState = 0;
	lcd->cursorPosition = 0;
	
	strncpy(lcd->line1, " Pos:   ,  ,    Mag:    ", 24);
	strncpy(lcd->line2, "Mode:          Time:    ", 24);
	
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
	writeControlRegister(lcd, 0x0C);	//Set display on
	writeControlRegister(lcd, 0x01);	//Clear display
	
	writeToLcdPosition(lcd, lcd->line1, sizeof(lcd->line1)/sizeof(lcd->line1[0]), 0);
	writeToLcdPosition(lcd, lcd->line2, sizeof(lcd->line2)/sizeof(lcd->line2[0]), LCD_SECOND_ROW_STARTING_ADDRESS);
}


/*!
 @brief Clears the LCD.
 @param *lcd: pointer to Lcd structure.
 @retval None
 */
void clearLcd(struct Lcd *lcd) {
	writeControlRegister(lcd, 0x01);
	lcd->cursorPosition = 0;
}

/*!
 @brief Writes text to the LCD. 
 @param *lcd: pointer to Lcd structure.
 @param *text: pointer to a c-string. 
 @param length: length of string to write to LCD
 */
void writeToLcd(struct Lcd *lcd, char text[], uint32_t length) {
	uint32_t i;
	clearLcd(lcd);
	for (i=0; i<length; i++) {
		writeDataRegister(lcd, text[i]);
	}
}


/*!
 @brief Writes text to the LCD at the indicated position.
 @param *lcd: pointer to Lcd structure.
 @param *text: pointer to a c-string. 
 @param length: length of string to write to LCD
 @param position: position in LCD where we need to write the c-string.
 @retval None
 */
void writeToLcdPosition(struct Lcd *lcd, char text[], uint32_t length, uint32_t position) {
	uint32_t i;
	//if (lcd->cursorPosition != position) {
		updatePosition(lcd, position);
	//}
	for (i=0; i<length; i++) {
		writeDataRegister(lcd, text[i]);
	}
}

/*!
 @brief Displays various information on the LCD, such as the position, angle, magnet state (ON/OFF), user mode (manual, accelerometer), time left.
 @param *lcd: pointer to Lcd structure.
 @retval None
 */
void updateLcdData(struct Lcd *lcd, uint32_t y, uint32_t z, uint32_t angle, uint32_t magnet, uint32_t mode, uint32_t time) {
	
	readControlRegister(lcd);
	uint8_t previousCursorPosition = lcd->cursorPosition;
	
	intToAscii(y, lcd->line1+6);
	intToAscii(z, lcd->line1+9);
	intToAscii(angle, lcd->line1+12);
	if (magnet) {
		strncpy(lcd->line1+21, "On ", 3);
	} else {
		strncpy(lcd->line1+21, "Off", 3);
	}
	if (mode) {
		strncpy(lcd->line2+6, "Keypad            ", 18);
	} else {
		strncpy(lcd->line2+6, "Accel     Time:   ", 18);
		intToAscii(time, lcd->line2+22);
	}
	
	writeToLcdPosition(lcd, lcd->line1, sizeof(lcd->line1)/sizeof(lcd->line1[0]), 0);
	writeToLcdPosition(lcd, lcd->line2, sizeof(lcd->line2)/sizeof(lcd->line2[0]), LCD_SECOND_ROW_STARTING_ADDRESS);
	updatePosition(lcd, previousCursorPosition);
	//osDelay(1);
	//writeDataRegister(lcd, '0');
	//readControlRegister(lcd);
	//osDelay(500);
}

/*!
 @brief Sets the cursor of the LCD at the desired position.
 @param *lcd: pointer to Lcd structure.
 @param position: position where we want to move LCD cursor
 @retval None
 */
void setCursor(struct Lcd *lcd, uint32_t position) {
	updatePosition(lcd, position);
	//osDelay(1);
	writeControlRegister(lcd, 0x0D);
}

/*!
 @brief Clears the cursor from the LCD.
 @param *lcd: pointer to Lcd structure.
 @retval None
 */
void clearCursor(struct Lcd *lcd) {
	writeControlRegister(lcd, 0x0C);
}

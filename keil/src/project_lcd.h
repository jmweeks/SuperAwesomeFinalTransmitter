#include "stm32f4xx.h"

#define LCD_WAIT_TIME 100
#define LCD_WIDTH 24
#define LCD_HEIGHT 2
#define LCD_SECOND_ROW_STARTING_ADDRESS 0x40

struct Lcd {
	uint8_t state;
	GPIO_TypeDef *GPIO;
	uint32_t periph;
	uint16_t pinRS;
	uint16_t pinRW;
	uint16_t pinE;
	uint16_t pinD4;
	uint16_t pinD5;
	uint16_t pinD6;
	uint16_t pinD7;
	uint32_t inReadInitState;
	uint16_t cursorPosition;
};

void init_lcd(struct Lcd *lcd);
void clearLcd(struct Lcd *lcd);
void writeToLcd(struct Lcd *lcd, char text[], uint32_t length);
void writeToLcdPosition(struct Lcd *lcd, char text[], uint32_t length, uint32_t position);

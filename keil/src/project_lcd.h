/*!
	@file project_lcd.h
	Header file that supports @ref project_lcd.c file.
*/
#include "stm32f4xx.h"

#define LCD_WAIT_TIME 0
#define LCD_WIDTH 24
#define LCD_HEIGHT 2
#define LCD_SECOND_ROW_STARTING_ADDRESS 0x40

/*!
 @brief Structure representing the LCD.
 */
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
	uint8_t cursorPosition;
	char line1[LCD_WIDTH];
	char line2[LCD_WIDTH];
};

/*!
 @brief Structure grouping the LCD initialization parameters.
 */
struct LcdInit {
	GPIO_TypeDef *GPIO;
	uint32_t periph;
	uint16_t pinRS;
	uint16_t pinRW;
	uint16_t pinE;
	uint16_t pinD4;
	uint16_t pinD5;
	uint16_t pinD6;
	uint16_t pinD7;
};

void init_lcd(struct Lcd *lcd, struct LcdInit *lcdInit);
void clearLcd(struct Lcd *lcd);
void writeToLcd(struct Lcd *lcd, char text[], uint32_t length);
void writeToLcdPosition(struct Lcd *lcd, char text[], uint32_t length, uint32_t position);
void updateLcdData(struct Lcd *lcd, uint32_t y, uint32_t z, uint32_t angle, uint32_t magnet, uint32_t mode, uint32_t time);
void setCursor(struct Lcd *lcd, uint32_t position);
void clearCursor(struct Lcd *lcd);

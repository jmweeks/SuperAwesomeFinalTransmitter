#include "stm32f4xx.h"


struct Keypad {
	uint16_t row_output;
	uint16_t column_output;
	uint16_t key_press;
	uint32_t key_char;
};
void init_keypad(struct Keypad *keypad);

void init_keypad_row_output(void);

void init_keypad_column_output(void);

void get_button_press(struct Keypad *keypad);

void get_keypad_entry(struct Keypad *keypad);

void init_TIM5(void);



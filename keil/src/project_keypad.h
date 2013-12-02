#include "stm32f4xx.h"
#include "cmsis_os.h"

struct Keypad {
	uint16_t row_output;
	uint16_t column_output;
	uint16_t key_press;
	uint32_t key_char;
	uint32_t new_data_available;
	uint16_t old_key_char;
	osThreadId threadID;
	osMutexId mutexID;
	
	GPIO_TypeDef *GPIO;
	uint32_t periph;
	uint16_t rowPins;
	uint16_t colPins;
};

struct KeypadInit {
	GPIO_TypeDef *GPIO;
	uint32_t periph;
	uint16_t rowPins;
	uint16_t colPins;
};

void init_keypad(struct Keypad *keypad, osThreadId **tid_thread_keypad, struct KeypadInit *keypadInit);

void keypadThread(void const *argument);

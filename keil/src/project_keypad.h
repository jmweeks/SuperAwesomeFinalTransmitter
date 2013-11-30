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
};

void init_keypad(struct Keypad *keypad, osThreadId **tid_thread_keypad);

void keypadThread(void const *argument);

#include "stm32f4xx.h"
#include "cmsis_os.h"

#define KEYPAD_SAMPLE_FREQUENCY 25

struct Keypad {
	uint16_t row_output;
	uint16_t column_output;
	uint16_t key_press;
	uint32_t key_char;
	uint32_t new_data;
	osThreadId threadID;
	osMutexId mutexID;
};

void init_keypad(struct Keypad *keypad, osThreadId **tid_thread_keypad);
void get_keypad_entry(struct Keypad *keypad);

void keypadThread(void const *argument);

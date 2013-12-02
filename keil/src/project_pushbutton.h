#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_init.h"

#define PUSHBUTTON_DELAY 50

struct Pushbutton {
	uint32_t buttonPressed;
	osThreadId threadID;
	osMutexId mutexID;
};

void init_pushbutton(struct Pushbutton *pushbutton, osThreadId **tid_thread_pushbutton);
void pushbuttonThread(void const *argument);

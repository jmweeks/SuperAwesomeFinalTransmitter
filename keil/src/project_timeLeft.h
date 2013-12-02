#include "stm32f4xx.h"
#include "cmsis_os.h"

#define STARTING_TIME 60

struct TimeLeft {
	uint32_t initialTime;
	uint32_t timeLeft;
	uint32_t started;
	osThreadId threadID;
	osMutexId mutexID;
};

void init_timeLeft(struct TimeLeft *timeLeft, osThreadId **tid_thread_timeLeft);
void timeLeftThread(void const *argument);
void resetTimeLeft(struct TimeLeft *timeLeft);

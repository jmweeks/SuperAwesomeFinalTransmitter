/*!
	@file project_timeLeft.h
	Header file that supports @ref project_timeLeft.c
*/
#include "stm32f4xx.h"
#include "cmsis_os.h"

#define STARTING_TIME 99

/**  
  * @brief	TimeLeft structure used to keep track of how much time there is left until the game is over.
  */
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

/*!
	@file project_timeLeft.c
	Source file for the countdown of the coin-collection game.
*/
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_init.h"
#include "project_timeLeft.h"

/*!
 @brief Function that initializes the countdown timer for the coin-collection game.
 @param *timeLeft: pointer to a TimeLeft structure
 @param **tid_thread_timeLeft: Pointer to a thread control block.
 @retval None
 */
void init_timeLeft(struct TimeLeft *timeLeft, osThreadId **tid_thread_timeLeft) {
	timeLeft->initialTime = STARTING_TIME;
	timeLeft->timeLeft = timeLeft->initialTime;
	timeLeft->started = 0;
	
	osThreadDef(timeLeftThread, osPriorityNormal, 1, 0);
	timeLeft->threadID = osThreadCreate(osThread(timeLeftThread), timeLeft);
	osMutexDef(timeLeftMutex);
	timeLeft->mutexID=osMutexCreate(osMutex(timeLeftMutex));
	
	*tid_thread_timeLeft = &(timeLeft->threadID);
}

/*!
 @brief Thread that counts down how much time there is left until the game ends.
 @param *argument: pointer to a TimeLeft structure passed to thread.
 @retval None
 */
void timeLeftThread (void const *argument) {
	struct TimeLeft *timeLeft;
	timeLeft = (struct TimeLeft *)argument;
	while(1) {
		osSignalWait(0x0001, osWaitForever);
		osMutexWait(timeLeft->mutexID, osWaitForever);
		if (timeLeft->started) {
			if (timeLeft->timeLeft > 0) {
				timeLeft->timeLeft--;
			}
		}
		osMutexRelease(timeLeft->mutexID);
	}
}

/*!
 @brief Function that resets the timer for the game.
 @param *timeLeft: pointer to a TimeLeft structure.
 @retval None
 */
void resetTimeLeft(struct TimeLeft *timeLeft) {
	osMutexWait(timeLeft->mutexID, osWaitForever);
	timeLeft->timeLeft = timeLeft->initialTime;
	timeLeft->started = 0;
	osMutexRelease(timeLeft->mutexID);
}

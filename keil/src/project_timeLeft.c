#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_init.h"
#include "project_timeLeft.h"

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

void resetTimeLeft(struct TimeLeft *timeLeft) {
	timeLeft->timeLeft = timeLeft->initialTime;
	timeLeft->started = 0;
}

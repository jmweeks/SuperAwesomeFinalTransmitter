#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_transmitter.h"
#include "project_init.h"
#include "CC2500.h"

void init_transmitter(struct Transmitter *transmitter, osThreadId **tid_thread_transmitter) {
	transmitter->state = 0;
	transmitter->buffer_space = 0;
	
	uint32_t i;
	for (i=0; i<sizeof(transmitter->data)/sizeof(transmitter->data[0]); i++) {
		transmitter->data[i] = 0;
	}
	
	CC2500_Init();
	CC2500_config_transmitter();
	goToTX(&(transmitter->state), &(transmitter->buffer_space));
	
	osThreadDef(transmitterThread, osPriorityNormal, 1, 0);
	transmitter->threadID = osThreadCreate(osThread(transmitterThread), transmitter);
	
	osMutexDef(transmitterMutex);
	transmitter->mutexID=osMutexCreate(osMutex(transmitterMutex));
	
	*tid_thread_transmitter = &(transmitter->threadID);
	
	init_TIM3();
}

void transmitterThread (void const *argument) {
	struct Transmitter *transmitter;
	transmitter = (struct Transmitter *)argument;
	
	while(1) {
		osSignalWait(0x0001, osWaitForever);
		osMutexWait(transmitter->mutexID, osWaitForever);
		wireless_TX(transmitter->data, TRANSMIT_DATA_SIZE, &(transmitter->state), &(transmitter->buffer_space));
		osMutexRelease(transmitter->mutexID);
	}
}

#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "CC2500.h"

#define RECEIVE_DATA_SIZE 5

#ifndef RECEIVER_STRUCT
#define RECEIVER_STRUCT

struct Receiver {
	uint8_t state;
	uint8_t buffer_space;
	uint8_t data[RECEIVE_DATA_SIZE];
	osMutexId mutexID;
};

#endif

void init_receiver(struct Receiver *receiver);

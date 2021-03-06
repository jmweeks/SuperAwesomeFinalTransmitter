/*!
	@file project_transmitter.h
	Header file that supports @ref project_transmitter.c
*/
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "CC2500.h"

#define TRANSMIT_DATA_SIZE 5

#ifndef TRANSMITTER_STRUCT
#define TRANSMITTER_STRUCT

/*!
 @brief Structure representing the transmitter state and buffer space, among other things.
 */
struct Transmitter {
	uint8_t data[TRANSMIT_DATA_SIZE];
	uint8_t state;
	uint8_t buffer_space;
	osThreadId threadID;
	osMutexId mutexID;
};

#endif

void init_transmitter(struct Transmitter *tranmistter, osThreadId **tid_thread_transmitter);

void transmitterThread(void const *argument);

/*!
	@file project_pushbutton.c
	Source file for the pushbutton functionality.
*/

#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_pushbutton.h"
#include "project_init.h"

/**  
  * @brief  Function that initializes the push button.
  * @param  *pushbutton: Pointer to a Pushbutton structure 
	* @param  *tid_thread_pushbutton: Pointer to the pushbutton thread control block.
  * @retval None  
  */
void init_pushbutton(struct Pushbutton *pushbutton, osThreadId **tid_thread_pushbutton) {
	pushbutton->buttonPressed = 0;
	
	osThreadDef(pushbuttonThread, osPriorityNormal, 1, 0);
	pushbutton->threadID = osThreadCreate(osThread(pushbuttonThread), pushbutton);
	
	osMutexDef(pushbuttonMutex);
	pushbutton->mutexID = osMutexCreate(osMutex(pushbuttonMutex));
	
	*tid_thread_pushbutton = &(pushbutton->threadID);
}


/**  
  * @brief  Thread that waits for the button push event. Once this event is received, 
	*					the state of the push button structure is updated.
  * @param  *argument: Pointer to a Pushbutton structure  
  * @retval None  
  */
void pushbuttonThread (void const *argument) {
	struct Pushbutton *pushbutton;
	pushbutton = (struct Pushbutton *)argument;
	
	while(1) {
		osSignalWait(0x0001, osWaitForever);
		osDelay(PUSHBUTTON_DELAY);																				//delay 10ms (debounce)
		while (GPIO_ReadInputDataBit(PUSHBUTTON_GPIO, PUSHBUTTON_PIN)) {	//reading pushbutton input
			osDelay(PUSHBUTTON_DELAY);																			//delay 10ms (debounce)
		}
		osMutexWait(pushbutton->mutexID, osWaitForever);
		pushbutton->buttonPressed = 1;
		osMutexRelease(pushbutton->mutexID);
		osSignalClear(pushbutton->threadID, 0x0001);
	}
}

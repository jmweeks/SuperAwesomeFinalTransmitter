#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_servo.h"

static void update_position(struct Servo *servo, uint32_t position) {
	if (position > servo->maxPosition) {
		position = servo->maxPosition;
	}
	if (position < servo->minPosition) {
		position = servo->minPosition;
	}
	uint32_t CCR_value = ((position * (SERVO_MAX_DUTY_CYCLE - SERVO_MIN_DUTY_CYCLE) / SERVO_STEPS) + SERVO_MIN_DUTY_CYCLE) / SERVO_DUTY_CYCLE_STEP;
	switch (servo->CCR) {
		case 1:
			TIM4->CCR1 = CCR_value;
			break;
		case 2:
			TIM4->CCR2 = CCR_value;
			break;
		case 3:
			TIM4->CCR3 = CCR_value;
			break;
		default:
			TIM4->CCR4 = CCR_value;
	}
	servo->realPosition = position;
}

void init_servo(struct Servo *servo, uint32_t CCR, uint32_t maxPosition, uint32_t minPosition) {
	servo->CCR = CCR;
	
	if (maxPosition > SERVO_MAX_POSITION) {
		servo->maxPosition = SERVO_MAX_POSITION;
	} else if (maxPosition < SERVO_MIN_POSITION) {
		servo->maxPosition = SERVO_MIN_POSITION;
	} else {
		servo->maxPosition = maxPosition;
	}
	
	if (minPosition > servo->maxPosition) {
		servo->minPosition = servo->maxPosition;
	} else if (minPosition < SERVO_MIN_POSITION) {
		servo->minPosition = SERVO_MIN_POSITION;
	} else {
		servo->minPosition = minPosition;
	}
	
	servo->position = (servo->maxPosition+servo->minPosition)/2;
	servo->realPosition = (servo->maxPosition+servo->minPosition)/2;
	
	osThreadDef(servoThread, osPriorityNormal, 1, 0);
	servo->threadID = osThreadCreate(osThread(servoThread), servo);
	osMutexDef(servoMutex);
	servo->mutexID=osMutexCreate(osMutex(servoMutex));
}

void moveServo(struct Servo *servo, uint32_t position) {
	osMutexWait(servo->mutexID, osWaitForever);
	servo->position = position;
	osMutexRelease(servo->mutexID);
}

void waitUntilServoStopped(struct Servo *servo) {
	osMutexWait(servo->mutexID, osWaitForever);
	while (servo->position != servo->realPosition) {
		osDelay(250);
	}
	osMutexRelease(servo->mutexID);
}

void floatServo(struct Servo *servo) {
	switch (servo->CCR) {
		case 1:
			TIM4->CCR1 = 0;
			break;
		case 2:
			TIM4->CCR2 = 0;
			break;
		case 3:
			TIM4->CCR3 = 0;
			break;
		default:
			TIM4->CCR4 = 0;
	}
}

void servoThread (void const *argument) {
	struct Servo *servo;
	servo = (struct Servo *)argument;
	while(1) {
		osMutexWait(servo->mutexID, osWaitForever);
		if (servo->position != servo->realPosition) {
			if (servo->position > servo->realPosition) {
				servo->realPosition++;
			} else {
				servo->realPosition--;
			}
			update_position(servo, servo->realPosition);
		}
		osMutexRelease(servo->mutexID);
		osDelay(1000 / SERVO_DEG_PER_SECOND);
	}
}

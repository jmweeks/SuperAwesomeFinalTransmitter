#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_pwm.h"

#define SERVO_STEPS 180
#define SERVO_FREQUENCY 50
#define SERVO_MIN_DUTY_CYCLE 0.03f
#define SERVO_MAX_DUTY_CYCLE 0.125f
#define SERVO_MAX_ROTATION_ANGLE SERVO_STEPS
#define SERVO_DUTY_CYCLE_STEP ((SERVO_MAX_DUTY_CYCLE-SERVO_MIN_DUTY_CYCLE)/SERVO_STEPS)
#define SERVO_DEG_PER_SECOND 25
#define SERVO_MIN_POSITION 0
#define SERVO_MAX_POSITION SERVO_MAX_ROTATION_ANGLE
#define SERVO_MOVING_WAIT 250

#ifndef SERVO_STRUCT_DEF
#define SERVO_STRUCT_DEF

struct Servo {
	uint32_t CCR;
	uint32_t position;
	uint32_t realPosition;
	uint32_t maxPosition;
	uint32_t minPosition;
	osThreadId threadID;
	osMutexId mutexID;
};

#endif

void init_servo(struct Servo *servo, uint32_t CCR, uint32_t maxPosition, uint32_t minPosition);
void moveServo(struct Servo *servo, uint32_t position);
void servoThread(void const *argument);
void waitUntilServoStopped(struct Servo *servo);
void floatServo(struct Servo *servo);

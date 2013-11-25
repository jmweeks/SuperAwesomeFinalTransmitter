#include "stm32f4xx.h"
#include <math.h>

#include "project_robot.h"

static void updateRobotPosition(struct Robot *robot) {
	if (robot->positionY < POSITION_Y_MIN) {
		robot->positionY = POSITION_Y_MIN;
	} else if (robot->positionY > POSITION_Y_MAX) {
		robot->positionY = POSITION_Y_MAX;
	}
	if (robot->positionZ < POSITION_Z_MIN) {
		robot->positionZ = POSITION_Z_MIN;
	} else if (robot->positionZ > POSITION_Z_MAX) {
		robot->positionZ = POSITION_Z_MAX;
	}
	if (robot->angle < POSITION_SWING_MIN) {
		robot->angle = POSITION_SWING_MIN;
	} else if (robot->angle > POSITION_SWING_MAX) {
		robot->angle = POSITION_SWING_MAX;
	}
	if (robot->positionZ) {
		robot->boomPosition = BOOM_UP_POSITION;
		switch (robot->positionY) {
			case 0:
				robot->crowdPosition = CROWD_POSITION_0;
				break;
			case 1:
				robot->crowdPosition = CROWD_POSITION_1;
				break;
			case 2:
				robot->crowdPosition = CROWD_POSITION_2;
				break;
			case 3:
				robot->crowdPosition = CROWD_POSITION_3;
				break;
			case 4:
				robot->crowdPosition = CROWD_POSITION_4;
				break;
			case 5:
				robot->crowdPosition = CROWD_POSITION_5;
				break;
			default:
				robot->crowdPosition = CROWD_POSITION_6;
		}
	} else {
		switch (robot->positionY) {
			case 0:
				robot->boomPosition = BOOM_DOWN_POSITION_0;
				robot->crowdPosition = CROWD_POSITION_0;
				break;
			case 1:
				robot->boomPosition = BOOM_DOWN_POSITION_1;
				robot->crowdPosition = CROWD_POSITION_1;
				break;
			case 2:
				robot->boomPosition = BOOM_DOWN_POSITION_2;
				robot->crowdPosition = CROWD_POSITION_2;
				break;
			case 3:
				robot->boomPosition = BOOM_DOWN_POSITION_3;
				robot->crowdPosition = CROWD_POSITION_3;
				break;
			case 4:
				robot->boomPosition = BOOM_DOWN_POSITION_4;
				robot->crowdPosition = CROWD_POSITION_4;
				break;
			case 5:
				robot->boomPosition = BOOM_DOWN_POSITION_5;
				robot->crowdPosition = CROWD_POSITION_5;
				break;
			default:
				robot->boomPosition = BOOM_DOWN_POSITION_6;
				robot->crowdPosition = CROWD_POSITION_6;
		}
	}
	switch (robot->angle) {
		case 0:
			robot->swingPosition = SWING_POSITION_0;
			break;
		case 1:
			robot->swingPosition = SWING_POSITION_1;
			break;
		case 2:
			robot->swingPosition = SWING_POSITION_2;
			break;
		case 3:
			robot->swingPosition = SWING_POSITION_3;
			break;
		case 4:
			robot->swingPosition = SWING_POSITION_4;
			break;
		case 5:
			robot->swingPosition = SWING_POSITION_5;
			break;
		case 6:
			robot->swingPosition = SWING_POSITION_6;
			break;
		case 7:
			robot->swingPosition = SWING_POSITION_7;
			break;
		case 8:
			robot->swingPosition = SWING_POSITION_8;
			break;
		case 9:
			robot->swingPosition = SWING_POSITION_9;
			break;
		case 10:
			robot->swingPosition = SWING_POSITION_10;
			break;
		case 11:
			robot->swingPosition = SWING_POSITION_11;
			break;
		case 12:
			robot->swingPosition = SWING_POSITION_12;
			break;
		default:
			robot->swingPosition = SWING_POSITION_13;
	}
}

void waitUntilRobotStopped(struct Robot *robot) {
	waitUntilServoStopped(robot->boomServo);
	waitUntilServoStopped(robot->crowdServo);
	waitUntilServoStopped(robot->swingServo);
}

void init_robot(struct Robot *robot, struct Servo *boomServo, struct Servo *crowdServo, struct Servo *swingServo) {
	robot->boomServo = boomServo;
	robot->crowdServo = crowdServo;
	robot->swingServo = swingServo;
	
	robot->boomPosition = robot->boomServo->minPosition;
	robot->crowdPosition = robot->crowdServo->minPosition;
	robot->swingPosition = robot->swingServo->minPosition;
	
	robot->positionY = 0;
	robot->positionZ = 0;
	robot->angle = 0;
}

void moveRobot(struct Robot *robot, uint32_t positionY, uint32_t positionZ, uint32_t angle) {
	robot->positionY = positionY;
	robot->positionZ = positionZ;
	robot->angle = angle;
	
	updateRobotPosition(robot);
	
	moveServo(robot->boomServo, robot->boomServo->maxPosition);
	waitUntilServoStopped(robot->boomServo);
	
	moveServo(robot->crowdServo, robot->crowdPosition);
	moveServo(robot->swingServo, robot->swingPosition);
	waitUntilRobotStopped(robot);
	
	moveServo(robot->boomServo, robot->boomPosition);
	waitUntilServoStopped(robot->boomServo);
}

void floatRobot(struct Robot *robot) {
	floatServo(robot->boomServo);
	floatServo(robot->crowdServo);
	floatServo(robot->swingServo);
}

void parkRobot(struct Robot *robot) {
	moveRobot(robot, PARK_Y, PARK_Z, PARK_ANGLE);
}

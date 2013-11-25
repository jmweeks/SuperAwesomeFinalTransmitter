#include "stm32f4xx.h"

#include "project_magnet.h"

void init_magnet(struct Magnet *magnet, uint32_t periph, GPIO_TypeDef *GPIOx, uint16_t pin) {
	magnet->GPIO = GPIOx;
	magnet->periph = periph;
	magnet->pin = pin;
	magnet->isOn = 0;
	
	RCC_AHB1PeriphClockCmd(magnet->periph, ENABLE);																			//Enable clock to GPIOD
	
	GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = magnet->pin;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																							//Aternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																					//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																						//Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;																					//No pull - output
  GPIO_Init(magnet->GPIO, &GPIO_InitStructure);																										//Pass struct and initialize
}

void turnMagnetOn(struct Magnet *magnet) {
	GPIO_SetBits(magnet->GPIO, magnet->pin);
	magnet->isOn = 1;
}

void turnMagnetOff(struct Magnet *magnet) {
	GPIO_ResetBits(magnet->GPIO, magnet->pin);
	magnet->isOn = 0;
}

void toggleMagnet(struct Magnet *magnet) {
	if (magnet->isOn) {
		turnMagnetOff(magnet);
	} else {
		turnMagnetOn(magnet);
	}
}

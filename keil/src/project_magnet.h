#include "stm32f4xx.h"

struct Magnet {
	GPIO_TypeDef *GPIO;
	uint32_t periph;
	uint16_t pin;
	uint32_t isOn;
};

void init_magnet(struct Magnet *magnet, uint32_t periph, GPIO_TypeDef *GPIOx, uint16_t pin);
void turnMagnetOn(struct Magnet *magnet);
void turnMagnetOff(struct Magnet *magnet);
void toggleMagnet(struct Magnet *magnet);

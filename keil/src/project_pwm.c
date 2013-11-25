#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_pwm.h"
#include "project_servo.h"

//For PWM

void init_TIM4(uint32_t period, uint32_t frequency) {
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);																			//Clock enable to TIM4
	
	TIM_TimeBaseStructure.TIM_Period = period;																															//Period = max intensity for pwm
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / (2 * frequency * period) - 1;		//Set prescaler for LED function
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;																																			//No clock divison
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;																												//Count up

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;																					//Set Output Compare mode for TIM4
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;															//Enable output
	TIM_OCInitStructure.TIM_Pulse = 0;																												//Set initial CCR value
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;																	//Set output active high

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);																									//Initialize TIM4 OC1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);																					//Enables TIM4 peripheral Preload register on CCR1
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);																									//Initialize TIM4 OC2
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);																					//Enables TIM4 peripheral Preload register on CCR2
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);																									//Initialize TIM4 OC3
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);																					//Enables TIM4 peripheral Preload register on CCR3
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);																									//Initialize TIM4 OC4
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);																					//Enables TIM4 peripheral Preload register on CCR4
	
  TIM_ARRPreloadConfig(TIM4, ENABLE);																												//Enable TIM4 Auto Reload Register 

  TIM_Cmd(TIM4, ENABLE);																																		//Enable TIM4
}

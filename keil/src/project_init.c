#include "stm32f4xx.h"

#include "project_init.h"

/**
  * @brief  Initialize USER pushbutton
	* @note   This function sets up the input GPIO for the USER pushbutton.
	*         The GPIO speed and direction are set.
	* @param  None
  * @retval None
  */

void init_pushbutton() {
	GPIO_InitTypeDef GPIO_InitStructure;																															//Initialize GPIO structure
	EXTI_InitTypeDef EXTI_InitStructure;																															//Initialize external interrupt structure
	NVIC_InitTypeDef NVIC_InitStructure;																															//Initialize nested vector interrupt controller structure
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);																						//Enable apb2 clock to syscfg, enable external interrupts
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 																						//Enable clock to Pushbutton
	
	GPIO_StructInit(&GPIO_InitStructure);																															//Initialize struct, reset to default values
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;																													//Specify which pin to configure
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;																											//Takes input data
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																									//Specify clock speed for pin
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																										//Specify operating output
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;																										//Specify resistor pull down
	GPIO_Init(GPIOA, &GPIO_InitStructure);			

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);																			//Connect/configure external interrupt on pin0 to be connected with GPIOA
	
	EXTI_StructInit(&EXTI_InitStructure);																															//Reset external interrupt structure
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;																												//Ext interrupt coming in on line 0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;																								//Set mode of the interrupt, external
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  																					//Ext interrupt triggered on a rising edge
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;																													//Enable new state of the interrupt
  EXTI_Init(&EXTI_InitStructure);																																		//Configure external interrupt with defined structure parameters
	
	/*The Nested Vectored Interrupt Controller (NVIC) offers very fast interrupt handling and provides the vector table as a set of real vectors (addresses).
	-Saves and restores automatically a set of the CPU registers (R0-R3, R12, PC, PSR, and LR).
	-Does a quick entry to the next pending interrupt without a complete pop/push sequence. (Tail-Chaining)
	-Serves a complete set of 255 (240 external) interrupts.
	*/
	
	// Preemption Priority = used to determine if an interrupt that occurs after can overtake
	// previous interrupt that is currently being serviced
	// SubPriority = used to determine priority if two interrupts occur at the same time
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;																									//Enable interrupt request channel 0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;																			//Set priority of pre-emption interrupt
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;																							//Set sub priority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;																										//Enable the IRQ channel specified by nvic
  NVIC_Init(&NVIC_InitStructure);																																		//Pass struct to NVIC, initialize
}

void init_TIM5() {
  NVIC_InitTypeDef NVIC_InitStructure;																								//NVIC initialization struct
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;																			//Timer initialization struct
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 					//Enable clock to TIM10
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;								//No clock division
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//Counts up
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;										//Period set for largest dynamic range of standard timer frequencies to be used
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/(2*TIM5_FREQUENCY*TIM_TimeBaseStructure.TIM_Period)-1;									//Set prescaler to determine frequency
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;						//Restart RCR count after counting down to this value
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);								//Initialize struct parameters to TIM5
	
	TIM_Cmd(TIM5, ENABLE);																				//Enable specified peripheral
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);										//Enable new interrupt state
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;								//Specify interrupt request channel to be used
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; 	//Indicates pre-emption priority, 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; 				//Subpriority value 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 							//Enable interrupt request channel specified earlier
  NVIC_Init(&NVIC_InitStructure);																//Initialize NVIC for TIM3 with struct params
}

void init_TIM3() {
  NVIC_InitTypeDef NVIC_InitStructure;																								//NVIC initialization struct
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;																			//Timer initialization struct
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 					//Enable clock to TIM10
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;								//No clock division
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//Counts up
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;										//Period set for largest dynamic range of standard timer frequencies to be used
	TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/(2*TIM3_FREQUENCY*TIM_TimeBaseStructure.TIM_Period)-1;									//Set prescaler to determine frequency
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;						//Restart RCR count after counting down to this value
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);								//Initialize struct parameters to TIM5
	
	TIM_Cmd(TIM3, ENABLE);																				//Enable specified peripheral
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);										//Enable new interrupt state
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;								//Specify interrupt request channel to be used
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; 	//Indicates pre-emption priority, 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; 				//Subpriority value 0-15, lower # =higher prriority
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 							//Enable interrupt request channel specified earlier
  NVIC_Init(&NVIC_InitStructure);																//Initialize NVIC for TIM3 with struct params
}

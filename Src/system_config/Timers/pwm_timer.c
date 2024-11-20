// Put implementations of PWM related functions in this file

#include "timers.h"
#include <globals.h>

void pwm_timer_gpio();
bool pwm_initTimer(uint32_t period_μs);
void pwm_setDutyCycle(float percentage);


void pwm_timer_gpio(){
	//Enable Clock & GPIO port
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN_Msk;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN_Msk;
	//Set MODER pins
	GPIOA->MODER |= GPIO_MODER_MODE15_1;
	//Set Alternate function
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL15_0;
}

bool pwm_initTimer(uint32_t period_μs){
	//clip value to valid bounds
	if (period_μs < 5000)
		period_μs = 5000;
	if (period_μs > 20000)
		period_μs = 20000;

	TIM2->EGR |= TIM_EGR_UG_Msk;
	//1. Configure the output pin:
	//a) Select the output mode by writing CCS bits in CCMRx register
	TIM2->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;
	//b) Select the polarity by writing the CCxP bit in CCER register.
	TIM2->CCER &= ~TIM_CCER_CC1P_Msk;
	//2. Select the PWM mode (PWM1 or PWM2) by writing OCxM bits in CCMRx register.
	TIM2->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2);
	//3. Program the period and the duty cycle respectively in ARR and CCRx registers.
	TIM2->ARR = (uint32_t)(period_μs * 5 - 1);
	//4. Set the pre-load bit in CCMRx register and the ARPE bit in the CR1 register.
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE_Msk;
	TIM2->CR1 |= TIM_CR1_ARPE_Msk;
	//5. Select the counting mode
	//DONT NEED TO DO ANYTHING??????
	//6. Enable the capture compare.
	TIM2->CCER |= TIM_CCER_CC1E_Msk;
	//7. Enable the counter.
	TIM2->CR1 &= ~(TIM_CR1_CMS_Msk | ~TIM_CR1_DIR_Msk);
	TIM2->CR1 |= TIM_CR1_CEN_Msk;

	TIM2->PSC = 1;
	//Puts TIM2 into  up-counting mode


	return true;
}

void pwm_setDutyCycle(float percentage){
	//clip value to 0-100
	if (percentage > 100)
		percentage = 100;
	if (percentage < 0)
		percentage = 0;
	percentage /= 100;

	TIM2->CCR1 = TIM2->ARR * percentage;
}

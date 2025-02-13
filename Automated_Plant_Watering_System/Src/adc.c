#include "stm32f4xx.h"
#include "adc.h"

#define GPIOBEN			(1U << 1)
#define ADC1EN			(1U << 8)
#define SQR3CHAN8		(8U << 0)
#define CONTCONV		(1U << 1)
#define ADC1SWSTART		(1U << 30)
#define ADC1EOC			(1U << 1)
#define CR2ADON			(1U << 0)
#define ADC_EOCEN		(1U << 5)


void pb0_adc_init(){
	/* Configure the ADC GPIO pin (PB0 --> GPIOB) */
	RCC->AHB1ENR |= GPIOBEN;

	/* Configure pin PB0
	 * Analog mode is (MSB) 11 (LSB)
	 * PB0 is bits 0 and 1 in the mode register
	*/
	GPIOB->MODER |= (1U << 0);
	GPIOB->MODER |= (1U << 1);

	/* Now, we enable clock access the ADC module */
	RCC->APB2ENR |= ADC1EN;

	/* Enable ADC end-of-conversion interrupt */
	ADC1->CR1 |= ADC_EOCEN;

	/* Enable the ADC Interrupt in NVIC */
	NVIC_EnableIRQ(ADC_IRQn);

	/* In SQR3, we assign sequence positions for our channels
	 * PB0 corresponds to ADC1 Channel 8
	 * We've created a symbolic name, now we can assign that to SQR3
	*/
	ADC1->SQR3 |= SQR3CHAN8;

	/* Now, we need to configure SQR1's L field, where we set the total number of conversions in the sequence */
	/* since it is 0-indexed, and we are doing 1 conversion, the value we set is 1-1 = 0. For ex: 6 conversions --> 6-1 = 5 */
	/* We can just set this whole thing to 0 */
	ADC1->SQR1 = 0x00;

	/* Finally, we enable the ADC conversion through the control register */
	ADC1->CR2 |= CR2ADON;

}

void start_conversion(void){
	/* Set bit 1 (continuous conversion bit) in ADC Control Register 2 */
	ADC1->CR2 |= CONTCONV;

	/* Begin the ADC conversion using the SWSTART bit */
	ADC1->CR2 |= ADC1SWSTART;
}

/* This function returns uint32_t (returns the value in the ADC1 DR) */
uint32_t adc_read(void){
	/* Wait for EOC (end of conversion flag) to be set in ADC1 Status Register */
	while(!(ADC1->SR & ADC1EOC)){
		/* Wait */
	}
	return (ADC1->DR);
}


























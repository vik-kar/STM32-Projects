#include "stm32f4xx.h"

#define GPIOAEN		(1U << 0)
#define PIN3_ON		(1U << 3)
#define PIN6_ON		(1U << 6)

void pa3_init(){
	/* PA3: Port A, pin 3 */
	/* Port A: GPIOA */

	/* Configure output mode */
	GPIOA->MODER |=  (1U << 6);
	GPIOA->MODER &=~ (1U << 7);
}

void pa3_on(){
	/* BSRR is reset after every execution, so no need for |= */
	GPIOA->BSRR = PIN3_ON;
}

void pa3_off(){
	/* Left shift by 16 to get into the reset range */
	GPIOA->BSRR = (PIN3_ON << 16);
}

void pa6_init(){
	RCC->AHB1ENR |= GPIOAEN;

	GPIOA->MODER |=  (1U << 12);
	GPIOA->MODER &=~ (1U << 13);
}

void pa6_on(){
	GPIOA->BSRR = PIN6_ON;
}

void pa6_off(){
	GPIOA->BSRR = (PIN6_ON << 16);
}

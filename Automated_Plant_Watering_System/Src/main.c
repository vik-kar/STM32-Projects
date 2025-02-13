#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "adc.h"
#include "togglePin.h"

uint32_t sensor_value;
uint32_t dry_threshold = 700;
uint32_t wet_threshold = 200;

int main(void){
	//pa6_off();
	pb0_adc_init();
	pa6_init();

	start_conversion();

	while(1){

	}
}

static void adc_callback(void){
	sensor_value = ADC1->DR;

	if(sensor_value >= 2500){
		pa6_on();
	}
	else if (sensor_value <= 1900){
		pa6_off();
	}

}

void ADC_IRQHandler(void){
	/* Check for ADC EOC flag */
	if((ADC1->SR & ADC1EOC) != 0){
		/* Clear EOC bit */
		ADC1->SR &=~ ADC1EOC;

		/* Execute callback */
		adc_callback();
	}
}




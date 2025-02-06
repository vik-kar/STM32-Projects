#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "adc.h"

uint32_t sensor_value;

int main(void){
	pb0_adc_init();

	start_conversion(); 

	while(1){
		sensor_value = adc_read();
	}
}




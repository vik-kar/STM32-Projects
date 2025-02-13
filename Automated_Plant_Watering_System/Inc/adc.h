/*
 * adc.h
 *
 *  Created on: Jan 8, 2025
 *      Author: vikramkarmarkar
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>

void pb0_adc_init();
void start_conversion(void);

#define ADC1EOC			(1U << 1)

uint32_t adc_read(void);

//bool motor_off = true;
//bool motor_on = false;

#endif /* ADC_H_ */

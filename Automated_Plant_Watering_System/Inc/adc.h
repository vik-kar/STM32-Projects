/*
 * adc.h
 *
 *  Created on: Jan 8, 2025
 *      Author: vikramkarmarkar
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
void pb0_adc_init();
void start_conversion(void);
uint32_t adc_read(void);

#endif /* ADC_H_ */

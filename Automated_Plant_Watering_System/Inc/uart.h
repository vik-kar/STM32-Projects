/*
 * uart.h
 *
 *  Created on: Nov 22, 2024
 *      Author: vikramkarmarkar
 */

#ifndef UART_H_
#define UART_H_
#include <stdint.h>
#include "stm32f4xx.h"


char uart2_read(void);

void uart2_rxtx_init(void);


#endif /* UART_H_ */

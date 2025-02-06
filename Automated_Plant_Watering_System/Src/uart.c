/*
 * uart.c
 *
 *  Created on: Nov 22, 2024
 *      Author: vikramkarmarkar
 */

#include "uart.h"

#define GPIOAEN		(1U << 0)
#define UART2EN		(1U << 17)

#define CR1_TE		(1U << 3)
#define CR1_RE		(1U << 2)

#define CR1_UE		(1U << 13)
#define SR_TXE		(1U << 7)
#define SR_RXNE		(1U << 5)

#define SYS_FREQ	16000000 //The default value for STM32 if we have not defined the clock tree (16000000 cycles per second)
#define APB1_CLK	SYS_FREQ

#define UART_BAUDRATE	115200

//add functions to the top so that they are accessible
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);


void uart2_write(int ch);
int __io_putchar(int ch);

int __io_putchar(int ch){
	uart2_write(ch);
	return ch;
}

/* This function will now initialize the transmitter and the receiver
 * PA3 is the rx line of our UART2
 *
*/

void uart2_rxtx_init(void){
	/*******************************Configure uart gpio pin*******************************/

	/*Enable clock access to gpioa*/
	RCC->AHB1ENR |= GPIOAEN;

	/*set PA2 mode to alternate function mode*/
	/*
	 * We need to visit GPIO port mode register - this register is a 32 bit register that determines the function of each pin in a specific GPIO port (ex GPIOA, GPIOB)
	 * Through this register, we can configure each pin as GPI/O, AF mode or analog mode
	 * We want to set MODER2 (for pin 2). '10' is AF mode. Pin 2 occupies bit 4 (0) and bit 5 (1)
	 */

	GPIOA->MODER &=~(1U << 4); //bit 4 to 0
	GPIOA->MODER |= (1U << 5); // bit 5 to 1

	/*set PA2 alternate function type to UART_TX (AF07)*/
	/*To access AFRL, we need to access element [0] of the AFR array*/
	GPIOA->AFR[0] |= 	(1U << 8);
	GPIOA->AFR[0] |= 	(1U << 9);
	GPIOA->AFR[0] |= 	(1U << 10);
	GPIOA->AFR[0] &=   ~(1U << 11);

	/*NEW: set PA3 mode to alternate function mode*/
	/*NEW: set PA3 mode type to UART_RX - PA3 occupies MODER3 (bits 6 and 7). bit 6 = 0, bit 7 = 1*/
	GPIOA->MODER &=~ (1U << 6); //bit 6 to 0
	GPIOA->MODER |=  (1U << 7); // bit 7 to 1

	/*Next: set PA3 alternate function type to UART_RX (AF07) using GPIO[x]_AFRL ---> AFRL3*/
	/* AFRL3: bit 12, 13, 14, 15, 0111: AF7 */
	GPIOA->AFR[0] |= 	(1U << 12);
	GPIOA->AFR[0] |= 	(1U << 13);
	GPIOA->AFR[0] |= 	(1U << 14);
	GPIOA->AFR[0] &=   ~(1U << 15);
	//DONE configuring pin 3 for UART_RX


	/*******************************Configure uart module*******************************/
	/*Enable clock access to UARt2 - remember, it is connected to the APB1 bus*/
	RCC->APB1ENR |= UART2EN;

	/*configure the UART buad rate*/
	/*
	 * why do we need a baud rate
	 * - it defines the speed of communication for the UART module - the baud rate sets how many bits are transmitted per second
	 *   over the UART connection
	 * - there is an equation to derive the baud rate, in a separate function
	 */
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	/*configure the transfer direction (we want TX)*/
	/*
	 * we need to go to the control register and see which bit enables the transmitter mode of the UART
	 */

	/* We also need to configure the receiver transfer direction */
	/* in reference manual: go to control register 1: bit 2 = RE (receiver enable) */

	USART2->CR1 = (CR1_TE | CR1_RE); //transmitter enable
	//we are not |=, because we want to clean the register (control register 1)
	//when we clean the rest of the register, it all becomes 0 and the settings we want for start/stop bits, parity, etc is all set by having 0s in the register




	/*Enable the uart module*/
	//also needs control register 1. Bit 13 is UE (UART enable)
	//we are doing |= here because we have already configured the transmitter (TE) so we do not want to wipe that out
	USART2->CR1 |= CR1_UE;

}

//this function reads the character
char uart2_read(void){
	/* make sure receiver data register is NOT empty
	 * look at RXNE flag in the status register
	 * RXNE is bit 5 in USART_SR
	 * 1: data is ready to be read
	 * 0: empty
	*/
	while(!(USART2->SR & SR_RXNE)){

	}

	/*read data */
	return USART2->DR;

}

void uart2_write(int ch){
	/*Make sure the transmit data register is empty before we try to put something into it
	 *
	 * - we need to check the UART status register for this
	 */

	//in parenthesis below: This will return true if the 7th bit is set inside the UART status register (SR)

	while(!(USART2->SR & SR_TXE)){
		//stuck here till the data register has space
	}

	/*Write to transmit data register*/

	//DR = data register
	/*
	 * & 0xFF operation ensures that only the lowest 8 bits (1 byte) of the value 'ch' are written to the data register
	 * UART transmission is typically byte-oriented, meaning only 8 bits can be transmitted at a time. (& 0xFF) masks out any higher-order bits that may exist
	 */
	USART2->DR = (ch & 0xFF); //transmit 8 bits
}

//The first argument works with any UART module (UART1, UART2, etc)
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate){
	USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate){
	//this returned value will be written into the UART baud rate register
	return ((PeriphClk + (BaudRate/2U)) / BaudRate);
}


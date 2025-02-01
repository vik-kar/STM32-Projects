#include "stm32f4xx.h"

#define GPIOBEN		(1U << 1)
#define I2C1EN		(1U << 21)

#define I2C_100KHZ			   80
#define SD_MODE_MAX_RISE_TIME  17

#define SR2_BUSY	(1U << 1)
#define CR1_START	(1U << 8)
#define SR1_SB		(1U << 0)
#define SR1_ADDR	(1U << 1)
#define SR1_TXE		(1U << 7)
#define CR1_ACK		(1U << 10)
#define CR1_STOP	(1U << 9)
#define SR1_RXNE	(1U << 6)
#define SR1_BTF		(1U << 2)

/*
 * Pinout
 * PB8 ---- SCL
 * PB9 ---- SDA
 */

void I2C1_init(void){
	/* Enable clock access to GPIOB (since we are using PB8/9)*/
	RCC->AHB1ENR |= GPIOBEN;

	/* Configure PB8 and PB9 to AF mode (AF04)*/

		/* PB8 */
	GPIOB->MODER &=~ (1U << 16); //0 for bit 16
	GPIOB->MODER |=  (1U << 17); //1 for bit 17

		/* PB9 */
	GPIOB->MODER &=~ (1U << 18); //0 for bit 18
	GPIOB->MODER |=  (1U << 19); //1 for bit 19

	/* Set PB8 and PB9 output type to open drain */
	GPIOB->OTYPER |= (1U << 8);
	GPIOB->OTYPER |= (1U << 9);

	/* Enable pullup for PB8 and PB9 */
	GPIOB->PUPDR |=  (1U << 16);
	GPIOB->PUPDR &=~ (1U << 17);

	GPIOB->PUPDR |=  (1U << 18);
	GPIOB->PUPDR &=~ (1U << 19);

	/* set AF04 mode for PB8 and PB9 */
	/* both pins are toggled to (LSB) 0010 (MSB) */
	GPIOB->AFR[1] &=~ (1U << 0);
	GPIOB->AFR[1] &=~ (1U << 1);
	GPIOB->AFR[1] |=  (1U << 2);
	GPIOB->AFR[1] &=~ (1U << 3);

	GPIOB->AFR[1] &=~ (1U << 4);
	GPIOB->AFR[1] &=~ (1U << 5);
	GPIOB->AFR[1] |=  (1U << 6);
	GPIOB->AFR[1] &=~ (1U << 7);

	/* Enable clock access to I2C1 through APB1 bus */
	RCC->APB1ENR |= I2C1EN;

	/* Enter reset mode for I2C */
	I2C1->CR1 |= (1U << 15);

	/* Come out of reset */
	I2C1->CR1 &=~ (1U << 15);

	/* Set clock to 16MHz */
	I2C1->CR2 = (1U << 4);

	/* Set CCR register value to 100KHz symbolic name */
	I2C1->CCR = I2C_100KHZ;

	/* Set the max trise value in the TRISE register */
	I2C1->TRISE = SD_MODE_MAX_RISE_TIME;

	/* Enable PE in I2C_CR1 (bit 0) */
	I2C1->CR1 |= (1U << 0);
}

void I2C1_byteRead(char saddr, char maddr, char* data){
	volatile int tmp;

	/* Wait for I2C status to NOT be busy */
	while(I2C1->SR2 & (SR2_BUSY)){
		/* Wait */
	}

	/* Begin the reading process */

	/* Generate start condition */
	I2C1->CR1 |= CR1_START;

	/* Wait for the start bit to be set in Control Register */
	while(!(I2C1->SR1 & (SR1_SB))){
		/* Wait */
	}

	/* Transmit slave address + write operation mode */
	/* Since 0 represents a write operation, we just ignore it */
	I2C1->DR = saddr << 1;

	/* Wait while the address is NOT set in the status register */
	while(!(I2C1->SR1 & SR1_ADDR)){
		/* Wait */
	}

	/* Clear the address flag */
	tmp = I2C1->SR2;

	/* Send the memory address through the Data Register */
	I2C1->DR = maddr;

	/* Wait while TXE in I2C_SR1 is 0 */
	while(!(I2C1->SR1 & SR1_TXE)){
		/* Wait */
	}

	/* Generate restart condition */
	I2C1->CR1 |= CR1_START;

	/* Once again, wait till the start flag is set */
	while(!(I2C1->SR1 & (SR1_SB))){
		/* Wait */
	}

	/* Transmit the slave address plus indicate read mode */
	I2C1->DR = saddr << 1 | 1;

	/* Wait till addr flag is set */
	while(!(I2C1->SR1 & SR1_ADDR)){
			/* Wait */
	}

	/* Disable ACK */
	I2C1->CR1 &=~ CR1_ACK;

	/* Clear addr flag */
	tmp = I2C1->SR2;

	/* Generate Stop condition */
	I2C1->CR1 |= CR1_STOP;

	/* Wait till RxNE is not empty */
	while(!(I2C1->SR1 & SR1_RXNE)){
		/* Wait till we have received something*/
	}

	/* Read the data register and store it in our variable */
	*data++ = I2C1->DR;
}

void I2C1_burstRead(char saddr, char maddr, int n, char* data){
	volatile int tmp;

	/* Wait for I2C status to NOT be busy */
	while(I2C1->SR2 & (SR2_BUSY)){
		/* Wait */
	}

	/* Generate start condition */
	I2C1->CR1 |= CR1_START;

	/* Wait for the start bit to be set in Control Register */
	while(!(I2C1->SR1 & (SR1_SB))){
		/* Wait */
	}

	/* Transmit slave address + write operation mode */
	/* Since 0 represents a write operation, we just ignore it */
	I2C1->DR = saddr << 1;

//	/* Wait while the address is NOT set in the status register */
	while(!(I2C1->SR1 & SR1_ADDR)){
		/* Wait */
	}

	/* Clear the address flag */
	tmp = I2C1->SR2;

	/* Wait while TXE in I2C_SR1 is 0 */
	/* This wait is NEW - it is always advisable to wait*/
	while(!(I2C1->SR1 & SR1_TXE)){
		/* Wait */
	}

	/* Send the memory address through the Data Register */
	I2C1->DR = maddr;

	/* Wait again while TXE in I2C_SR1 is 0 */
	while(!(I2C1->SR1 & SR1_TXE)){
		/* Wait */
	}

	/* send the restart condition */
	I2C1->CR1 |= CR1_START;

	/* Wait till start bit flag is set */
	while(!(I2C1->SR1 & (SR1_SB))){
		/* Wait */
	}

	/* Transmit the slave address plus indicate read mode */
	I2C1->DR = saddr << 1 | 1;

	/* Wait till addr flag is set */
	while(!(I2C1->SR1 & SR1_ADDR)){
			/* Wait */
	}

	/* Clear addr flag */
	tmp = I2C1->SR2;

	/* Enable ack for multi byte reading */
	I2C1->CR1 |= CR1_ACK;

	/* Now, begin the multi-byte reading process */
	while(n > 0U){
		/* Check if n = 1 */
		if(n == 1U){
			/* Disable ack, equivalent to sending NACK that tells sender no more bytes to read */
			I2C1->CR1 &=~ CR1_ACK;

			/* Generate stop */
			I2C1->CR1 |= CR1_STOP;

			/* Wait for RXNE Flag to be set (wait till the receive buffer is full) */
			while(!(I2C1->SR1 & SR1_RXNE)){
				/* Wait */
			}

			/* Read data from the data register */
			*data++ = I2C1->DR;

			break;
		}
		else{
			/* Wait for data to arrive (inspect RXNE) */
			while(!(I2C1->SR1 & SR1_RXNE)){
				/* Wait */
			}
			/* store it in data variable*/
			(*data++) = I2C1->DR;
			/* Decrement number of bytes to read (n) */
			n--;
		}
	}
}

void I2C1_burstWrite(char saddr, char maddr, int n, char* data){
	volatile int tmp;

	/* Wait for I2C status to NOT be busy */
	while(I2C1->SR2 & (SR2_BUSY)){
		/* Wait */
	}

	/* Generate start condition */
	I2C1->CR1 |= CR1_START;

	/* Wait for the start bit to be set in Control Register */
	while(!(I2C1->SR1 & (SR1_SB))){
		/* Wait */
	}

	/* Transmit slave address + write operation mode */
	/* Since 0 represents a write operation, we just ignore it */
	I2C1->DR = saddr << 1;

	/* Wait while the address is NOT set in the status register */
	while(!(I2C1->SR1 & SR1_ADDR)){
		/* Wait */
	}

	/* clear addr */
	tmp = I2C1->SR2;

	/* Wait while TXE in I2C_SR1 is 0 */
	while(!(I2C1->SR1 & SR1_TXE)){
		/* Wait */
	}

	/* Send the memory address through the Data Register */
	I2C1->DR = maddr;

	/* to write multiple bytes */
	for(int i = 0; i < n; i++){
		/* Wait until data register is empty (wait for TXE flag) */
		while(!(I2C1->SR1 & SR1_TXE)){
			/* wait */
		}

		/* Transmit data then increment */
		I2C1->DR = *data++;

	}

	/* Wait until transfer is finished (use BTF flag) */
	while(!(I2C1->SR1 & SR1_BTF)){
		/* Wait */
	}

	/* generate stop condition */
	I2C1->CR1 |= CR1_STOP;
}

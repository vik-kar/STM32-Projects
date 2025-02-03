#include "spi.h"

#define SPI1EN		(1U << 12)
#define GPIOAEN		(1U << 0)

#define SR_RXNE		(1U << 0)

//PA5 -> CLK
//PA6 -> MISO
//PA7 -> MOSI
//PA9 -> Slave Select


void spi_gpio_init(void){
	/* Enable clock access to GPIOA */
	RCC->AHB1ENR |= GPIOAEN;

	/* Configure pins to AF mode (first bit 0, second bit 1) */
	//PA5
	GPIOA->MODER &=~ (1U << 10);
	GPIOA->MODER |=  (1U << 11);

	//PA6
	GPIOA->MODER &=~ (1U << 12);
	GPIOA->MODER |=  (1U << 13);

	//PA7
	GPIOA->MODER &=~ (1U << 14);
	GPIOA->MODER |=  (1U << 15);

	//PA9 set to output mode (will go high and low to select slave device)
	//output mode: first bit 1 second bit 0
	GPIOA->MODER |=   (1U << 18);
	GPIOA->MODER &=~  (1U << 19);

	/* Set PA5, PA6, PA7 AF type to AF05 */
	// PA5
	GPIOA->AFR[0] |=  (1U << 20);
	GPIOA->AFR[0] &=~ (1U << 21);
	GPIOA->AFR[0] |=  (1U << 22);
	GPIOA->AFR[0] &=~ (1U << 23);

	//PA6
	GPIOA->AFR[0] |=  (1U << 24);
	GPIOA->AFR[0] &=~ (1U << 25);
	GPIOA->AFR[0] |=  (1U << 26);
	GPIOA->AFR[0] &=~ (1U << 27);

	//PA7
	GPIOA->AFR[0] |=  (1U << 28);
	GPIOA->AFR[0] &=~ (1U << 29);
	GPIOA->AFR[0] |=  (1U << 30);
	GPIOA->AFR[0] &=~ (1U << 31);
}

void spi1_config(void){
	/* Enable clock access to SPI1 module */
	RCC->APB2ENR |= SPI1EN;

	/* Set the clock of the SPI module (f_pclk)/4 */
	SPI1->CR1 |= (1U << 3);   //1
	SPI1->CR1 &=~ (1U << 4);  //0
	SPI1->CR1 &=~ (1U << 5);  //0

	/* Set clock polarity and clock phase */
	SPI1->CR1 |= (1U << 0);
	SPI1->CR1 |= (1U << 1);

	/* Set full duplex mode */
	SPI1->CR1 &=~ (1U << 10);

	/* Configure MSB first */
	SPI1->CR1 &=~ (1U << 7);

	/* Set to master mode */
	SPI1->CR1 |= (1U << 2);

	/* Set 8 bit frame format */
	SPI1->CR1 &=~ (1U << 11);

	/* Select software slave management */
	SPI1->CR1 |= (1 << 8);
	SPI1->CR1 |= (1 << 9);

	/* Enable SPI peripheral */
	SPI1->CR1 |= (1U << 6);
}

void spi1_transmit(uint8_t *data, uint32_t size){
	uint32_t i=0;
	uint8_t temp;

	while(i < size){
		/* Wait until TXE bit set in SPI (wait till transmit buffer is empty) */
		while(!(SPI1->SR & (1U << 1))){
			/* Wait */
		}

		/* Now, we know there is space, so we want to write data to the data register */
		SPI1->DR = data[i];
		i++;
	}

	/* Wait for TXE to be set again to indicate we've transmitted from our end */
	while(!(SPI1->SR & (1U << 1))){
		/* Wait */
	}

	/* Wait for not busy flag - while(1) get stuck - busy flag = 1 means busy*/
	while((SPI1->SR & (1U << 7))){
		/* Wait */
	}

	/* Clear OVR flag */
	temp = SPI1->DR;
	temp = SPI1->SR;
}

void spi1_receive(uint8_t *data, uint32_t size){
	/* While size is in bounds, keep reading */
	while(size){
		/* send dummy data */
		SPI1->DR = 0;

		while(!(SPI1->SR & SR_RXNE)){
			/* Wait */
		}

		/* Now, we read. RXNE Flag is now set - implies we have new data in the buffer to read */
		*data++ = SPI1->DR;
		size--;
	}
}

/* Indicates to the slave that data is coming */
void cs_enable(void){
	GPIOA->ODR &=~ (1U << 9);
}

/* Indicates end of transmission */
void cs_disable(void){
	GPIOA->ODR |= (1U << 9);
}

#include "adxl345.h"

#define MULTI_BYTE_EN		0x40
#define READ_OPERATION		0x80
char data;

void adxl_read_address (uint8_t address, uint8_t *rxdata){
	/* Set read operation */
	address |= READ_OPERATION;

	/* Enable multi-byte */
	address |= MULTI_BYTE_EN;

	/* Pull CS line low to indicate data is coming */
	cs_enable();

	/* Send address of the memory location that we want to read from */
	spi1_transmit(&address, 1);

	/* Store the received data in a buffer, read 6 bytes */
	spi1_receive(rxdata, 6);

	/* Disable slave device */
	cs_disable();

}

void adxl_write (uint8_t address, uint8_t value){
	uint8_t data[2];

	/* Enable multiple-byte flow */
	data[0] = address | MULTI_BYTE_EN;
	data[1] = value;

	/* Pull CS line low to enable the slave */
	cs_enable();

	/* Transmit data */
	spi1_transmit(data, sizeof(data));

	/* Pull CS line high to disable slave */
	cs_disable();
}

void adxl_init (void)
{
	/* Enable SPI GPIO */
	spi_gpio_init();

	/* Config SPI */
	spi1_config();

	/*Set data format range to +-4g*/
	adxl_write (DATA_FORMAT_R, FOUR_G);

	/*Reset all bits*/
	adxl_write (POWER_CTL_R, RESET);

	/*Configure power control measure bit*/
	adxl_write (POWER_CTL_R, SET_MEASURE_B);
}

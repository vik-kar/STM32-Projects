#include "adxl345.h"

char data;

uint8_t data_rec[6];

void adxl_read_address(uint8_t reg){
	I2C1_byteRead(DEVICE_ADDR, reg, &data);
}

void adxl_write (uint8_t reg, char value){
	/* create a local buffer to hold 'value' */
	char data[1];
	data[0] = value;

	/* slave address, location in slave, # of bytes to write, data to write */
	I2C1_burstWrite(DEVICE_ADDR, reg, 1, data);
}

void adxl_read_values(uint8_t reg){
	I2C1_burstRead(DEVICE_ADDR, reg, 6, (char *) data_rec);
}

void adxl_init (void){
	/* Enable I2C module */
	I2C1_init();
	/* read the DEVID, this should return 0xE5 (according to datasheet) */
	adxl_read_address(DEVID_R);

	/* set the data format range to +/- 4g */
	adxl_write(DATA_FORMAT_R, FOUR_G);

	/* reset all bits in the power control register */
	adxl_write(POWER_CTL_R, RESET);

	/* set the power control measure bit */
	adxl_write(POWER_CTL_R, SET_MEASURE_B);
}

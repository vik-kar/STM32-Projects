#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "adxl345.h"

/* Variables to combine DATA[x]0/1 values */
int16_t x,  y,  z;
float   xg, yg, zg;

extern uint8_t data_rec[6];

int main(void){
	adxl_init();

	while(1){
		adxl_read_values(DATA_START_ADDR);

		/* vCombine the x and y values to get one value. Do this via shifting */
		x = ((data_rec[1] << 8) | data_rec[0]);
		y = ((data_rec[3] << 8) | data_rec[2]);
		z = ((data_rec[5] << 8) | data_rec[4]);

		/* multiply by a scale factor to convert to g */
		xg = (x * 0.0078);
		yg = (y * 0.0078);
		zg = (z * 0.0078);
	}
}



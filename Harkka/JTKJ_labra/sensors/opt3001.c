/*
 * opt3001.c
 *
 *  Created on: 22.7.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/opt3001.pdf
 */

#include <string.h>
#include <math.h>

#include <xdc/runtime/System.h>

#include "Board.h"
#include "sensors/opt3001.h"

void opt3001_setup(I2C_Handle *i2c) {

	I2C_Transaction i2cTransaction;
	char itxBuffer[3];

    i2cTransaction.slaveAddress = Board_OPT3001_ADDR;
    itxBuffer[0] = OPT3001_REG_CONFIG;
    itxBuffer[1] = 0xCE; // continuous mode s.22
    itxBuffer[2] = 0x02;
    i2cTransaction.writeBuf = itxBuffer;
    i2cTransaction.writeCount = 3;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    if (I2C_transfer(*i2c, &i2cTransaction)) {

        System_printf("OPT3001: Config write ok\n");
    } else {
        System_printf("OPT3001: Config write failed!\n");
    }
    System_flush();

}

uint16_t opt3001_get_status(I2C_Handle *i2c) {

	uint16_t e=0;
	I2C_Transaction i2cTransaction;
	char itxBuffer[1];
	char irxBuffer[2];

	/* Read sensor state */
	i2cTransaction.slaveAddress = Board_OPT3001_ADDR;
	itxBuffer[0] = OPT3001_REG_CONFIG;
	i2cTransaction.writeBuf = itxBuffer;
	i2cTransaction.writeCount = 1;
	i2cTransaction.readBuf = irxBuffer;
	i2cTransaction.readCount = 2;

	if (I2C_transfer(*i2c, &i2cTransaction)) {

		e = (irxBuffer[0] << 8) | irxBuffer[1];
	} else {

		e = 0;
	}
	return e;
}

double opt3001_get_data(I2C_Handle *i2c) {

	double lux = -1.0;
	I2C_Transaction i2cTransaction;

	if (opt3001_get_status(i2c) & OPT3001_DATA_READY) {

		// JTKJ: Teht�v� 2. Kommentit pois ja t�ytet��n i2c-viestirakenne luentomateriaalin avulla..
		// JTKJ: Exercise 2. Uncomment and fill in the data structure below with correct values..

		// Viestipuskurit, esittele oikea koko (korvaa nn)
		uint8_t txBuffer[ 1 ];
		uint8_t rxBuffer[ 2 ];
		// Laitteen i2c-osoite
		i2cTransaction.slaveAddress = Board_OPT3001_ADDR;
		// Laitteen rekisterin osoite
		txBuffer[0] = OPT3001_REG_RESULT;
		// korvaa nn
		i2cTransaction.writeBuf = txBuffer;
		i2cTransaction.writeCount = 1;
		i2cTransaction.readBuf = rxBuffer;
		i2cTransaction.readCount = 2;


		if (I2C_transfer(*i2c, &i2cTransaction)) {

			// JTKJ: 1. Muunna mittausarvo 16-bit kokonaisluvuksi viestipuskurista
			//          (rxBuffer)
			//          Convert the register values (in rxBuffer) into 16 bit integer

			// JTKJ: 2. Laske mittausarvo luxeina luentomateriaalin (datakirjan)
			//          harjoitusteht�v�si kaavalla
			//          By using the equation in datasheet or your exercise
			//          find the value in lux

			uint16_t mask = 0b0000111111111111;
			uint16_t bits = (rxBuffer[0] << 8) | rxBuffer[1];
			uint16_t e0e3 = bits >> 12;
			uint16_t r0r11 = bits & ~(mask);
			lux = 0.01 * pow(2, e0e3) * r0r11;
            
		} else {

			System_printf("OPT3001: Data read failed!\n");
			System_flush();
		}
	} else {
		System_printf("OPT3001: Data not ready!\n");
		System_flush();
	}

	return lux;
}

/*
 * hdc1000.c
 *
 *  Created on: 22.7.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 * 	Datasheet http://www.ti.com/lit/ds/symlink/hdc1000.pdf
 */

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>

#include "Board.h"
#include "sensors/hdc1000.h"

void hdc1000_setup(I2C_Handle *i2c) {

    System_printf("HDC1000: Do not use this sensor!\n");
    System_flush();

	/*
	I2C_Transaction i2cTransaction;
	char itxBuffer[4];
	char irxBuffer[4];

    i2cTransaction.slaveAddress = Board_HDC1000_ADDR;
    itxBuffer[0] = HDC1000_REG_CONFIG;
    itxBuffer[1] = 0x10; // sequential mode s.16
    itxBuffer[2] = 0x00;
    i2cTransaction.writeBuf = itxBuffer;
    i2cTransaction.writeCount = 3;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    if (I2C_transfer(*i2c, &i2cTransaction)) {

        System_printf("HDC1000: Config write ok\n");
    } else {
        System_printf("HDC1000: Config write failed!\n");
    }
    */
}

void hdc1000_get_data(I2C_Handle *i2c, double *temp, double *hum) {

    System_printf("HDC1000: Do not use this sensor!\n");
    System_flush();

	// JTKJ: Keskeytyspohjaista anturidatan lukua ei ole toteutettu
	//       This functionality has not been implemented for the coursr project
	*temp = -1.0;
	*hum = - 1.0;
}


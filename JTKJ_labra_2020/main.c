#include <stdio.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "Board.h"

#include "wireless/comm_lib.h"
#include "sensors/opt3001.h"

/* Task */
#define STACKSIZE 2048
Char labTaskStack[STACKSIZE];
// Char commTaskStack[STACKSIZE];

// JTKJ: Teht�v� 1. Painonappien alustus ja muuttujat
// JTKJ: Exercise 1. Pin configuration and variables here

// JTKJ: Teht�v� 1.Painonapin keskeytyksen k�sittelij�
//       K�sittelij�ss� vilkuta punaista ledi�
// JTKJ: Exercise 1. Pin interrupt handler
//       Blink the red led of the device

/* Task Functions */
Void labTaskFxn(UArg arg0, UArg arg1) {

    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    Display_Handle	displayHandle;
    UART_Handle 	uartHandle;

    // JTKJ: Teht�v� 2. Avaa i2c-v�yl� taskin k�ytt��n
    // JTKJ: Exercise 2. Open the i2c bus0

    // JTKJ: Teht�v� 2. Sensorin alustus kirjastofunktiolla ennen datan lukemista
    // JTKJ: Exercise 2. Setup the sensor here, before its use

	// JTKJ: Teht�v� 3. N�yt�n alustus
    // JTKJ: Exercise 3. Setup the display here

    // JTKJ: Teht�v� 4. UARTin alustus
    // JTKJ: Exercise 4. Setup UART connection

    while (1) {

        // JTKJ: Teht�v� 2. Lue sensorilta dataa ja tulosta se Debug-ikkunaan
        // JTKJ: Exercise 2. Read sensor data and print it to the Debug window

    	// JTKJ: Teht�v� 3. Tulosta sensorin arvo merkkijonoon ja kirjoita se ruudulle
		// JTKJ: Exercise 3. Store the sensor value as char array and print it to the display

    	// JTKJ: Teht�v� 4. L�het� CSV-muotoinen merkkijono UARTilla
		// JTKJ: Exercise 4. Send CSV string with UART

    	// Once per second
    	Task_sleep(1000000 / Clock_tickPeriod);
    }
}

/* Communication Task */
/*
Void commTaskFxn(UArg arg0, UArg arg1) {

    // Radio to receive mode
	int32_t result = StartReceive6LoWPAN();
	if(result != true) {
		System_abort("Wireless receive mode failed");
	}

    while (1) {

        // If true, we have a message
    	if (GetRXFlag() == true) {

    		// Handle the received message..
        }

    	// Absolutely NO Task_sleep in this task!!
    }
}
*/

Int main(void) {

    // Task variables
	Task_Handle labTask;
	Task_Params labTaskParams;
	/*
	Task_Handle commTask;
	Task_Params commTaskParams;
	*/

    // Initialize board
    Board_initGeneral();

    // JTKJ: Teht�v� 2. i2c-c�yl� k�ytt��n ohjelmassa
    // JTKJ: Exercise 2. Use i2c bus in program

    // JTKJ: Teht�v� 4. UART k�ytt��n ohjelmassa
    // JTKJ: Exercise 4. Use UART in program

    // JTKJ: Teht�v� 1. Painonappi- ja ledipinnit k�ytt��n t�ss�
	// JTKJ: Exercise 1. Open and configure the button and led pins here

    // JTKJ: Teht�v� 1. Rekister�i painonapille keskeytyksen k�sittelij�funktio
	// JTKJ: Exercise 1. Register the interrupt handler for the button

    /* Task */
    Task_Params_init(&labTaskParams);
    labTaskParams.stackSize = STACKSIZE;
    labTaskParams.stack = &labTaskStack;
    labTaskParams.priority=2;

    labTask = Task_create(labTaskFxn, &labTaskParams, NULL);
    if (labTask == NULL) {
    	System_abort("Task create failed!");
    }

    /* Communication Task */
	/*
    Init6LoWPAN(); // This function call before use!

    Task_Params_init(&commTaskParams);
    commTaskParams.stackSize = STACKSIZE;
    commTaskParams.stack = &commTaskStack;
    commTaskParams.priority=1;

    commTask = Task_create(commTaskFxn, &commTaskParams, NULL);
    if (commTask == NULL) {
    	System_abort("Task create failed!");
    }
	*/

    /* Sanity check */
    System_printf("Hello world!\n");
    System_flush();
    
    /* Start BIOS */
    BIOS_start();

    return (0);
}

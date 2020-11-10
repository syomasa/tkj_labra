#include <stdio.h>
#include <string.h>

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
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "Board.h"
#include <kuva.h>
#include "buzzer.h"

#include "wireless/comm_lib.h"
#include "sensors/opt3001.h"
#include "sensors/mpu9250.h"

/* Task */
#define STACKSIZE 2048
Char labTaskStack[STACKSIZE];
Char defaultStateStack[STACKSIZE];
Char displayStack[STACKSIZE];
Char musicTaskStack[STACKSIZE];
// Char commTaskStack[STACKSIZE];

static PIN_Handle buzzer;
static PIN_State buzzerState;
static PIN_Handle hMpuPin;
static PIN_State MpuPinState;
static PIN_Config MpuPinConfig[] =
{
    Board_MPU_POWER  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

PIN_Config buzzerConfig[] =
{
    Board_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_PULLUP,
    PIN_TERMINATE
};

static const I2CCC26XX_I2CPinCfg i2cMPUCfg =
{
    .pinSDA = Board_I2C0_SDA1,
    .pinSCL = Board_I2C0_SCL1
};


enum state{START, STILL, LEFT, RIGHT, UP, DOWN};
enum state myState = STILL;

/* Task Functions */
Void labTaskFxn(UArg arg0, UArg arg1)
{


    I2C_Handle i2cMPU;
    I2C_Params i2cMPUParams;
    I2C_Params_init(&i2cMPUParams);
    i2cMPUParams.bitRate = I2C_400kHz;
    i2cMPUParams.custom = (uintptr_t)&i2cMPUCfg;

    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
    // checks if opening Mpu is succesful
    if (i2cMPU == NULL)
    {
    	System_abort("Error Initializing I2CMPU\n");
    }

    PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_ON);

    // WAIT 100MS FOR THE SENSOR TO POWER UP
    Task_sleep(100000 / Clock_tickPeriod);
    System_printf("MPU9250: Power ON\n");
    System_flush();


    System_printf("Started setting up MPU\n");
    System_flush();

    mpu9250_setup(&i2cMPU);

    System_printf("Mpu is ready now\n");
    System_flush();

    I2C_close(i2cMPU);
}

Void displayTask(UArg arg0, UArg arg1)
{
	Display_Params params;
	Display_Params_init(&params);
	params.lineClearMode = DISPLAY_CLEAR_BOTH;
    Display_Handle displayHandle = Display_open(Display_Type_LCD, &params);

		if(displayHandle)
		{
			tContext *pContext = DisplayExt_getGrlibContext(displayHandle);
			if (pContext)
		{
				while(1)
				{
					//if(myState == STILL)
				//{
					//Display_print0(displayHandle, 5, 0, "I'm still standing");

					//}
					if(myState == LEFT)
					{
						GrImageDraw(pContext, &arrowL, 0, 10);
						GrFlush(pContext);
						myState = STILL;
					}
					else if(myState == RIGHT)
					{
						GrImageDraw(pContext, &arrowR, 0, 10);
						GrFlush(pContext);
						myState = STILL;
					}
					else if(myState == UP)
					{
						GrImageDraw(pContext, &arrowU, 0, 10);
						GrFlush(pContext);
						myState = STILL;
					}
					else if(myState == DOWN)
					{
						GrImageDraw(pContext, &arrowD, 0, 10);
						GrFlush(pContext);
						myState = STILL;
					}
					Task_sleep(100000 / Clock_tickPeriod);
				}
			}
		}
	}

Void sensorTask(UArg arg0, UArg arg1)
{
	float ax, ay, az, gx, gy, gz;

	System_printf("Started sensorTask - function\n");
	System_flush();

	I2C_Handle i2cMPU;
	I2C_Params i2cMPUParams;
	I2C_Params_init(&i2cMPUParams);
	i2cMPUParams.bitRate = I2C_400kHz;
	i2cMPUParams.custom = (uintptr_t)&i2cMPUCfg;

	i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
	if (i2cMPU == NULL)
	{
		System_abort("Error Initializing I2CMPU\n");
	}

	PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_ON);

	// WAIT 100MS FOR THE SENSOR TO POWER UP
	Task_sleep(100000 / Clock_tickPeriod);
	System_printf("MPU9250: Power ON\n");
	System_flush();

	System_printf("Started setting up MPU\n");
	System_flush();

	mpu9250_setup(&i2cMPU);

	System_printf("Mpu is ready now\n");
	System_flush();

	I2C_close(i2cMPU);

	while (1) {

			if(myState == STILL){

				// MPU OPEN I2C
				i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
				if (i2cMPU == NULL) {
					System_abort("Error Initializing I2CMPU\n");
				}

				// MPU ASK DATA
				mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
			    // MPU CLOSE I2C
				I2C_close(i2cMPU);


				if(gx > 60) {
					myState = UP;
					displayTask(arg0, arg1);
				}
				else if(gx < -60) {
					myState = DOWN;
					displayTask(arg0, arg1);
				}
				else if(gy > 60) {
					myState = RIGHT;
					displayTask(arg0, arg1);
				}
				else if(gy < -60) {
					myState = LEFT;
					displayTask(arg0, arg1);
				}

				Task_sleep(500000 / Clock_tickPeriod);

		    }
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
	Task_Handle sensorTaskVar;
	Task_Params labTaskParams;

	Task_Handle displayTaskVar;
	Task_Params displayParams;

	//Task_Handle musicTask;
	//Task_Params musicTaskParams;


	/*
	Task_Handle commTask;
	Task_Params commTaskParams;
	*/

    // Initialize board
    Board_initGeneral();
    Board_initI2C();

    //buzzer = PIN_open(&buzzerState, buzzerConfig);
    //if (buzzer == NULL)
    //{
   // 	System_abort("Buzzer pin open failed!");
   // }

    /* Task */
    Task_Params_init(&labTaskParams);
    labTaskParams.stackSize = STACKSIZE;
    labTaskParams.stack = &labTaskStack;
    //labTaskParams.priority=3;

    Task_Params_init(&displayParams);
    displayParams.stackSize = STACKSIZE;
    displayParams.stack = &displayStack;
    //displayParams.priority = 2;

    //Task_Params_init(&musicTaskParams);
 	//musicTaskParams.stackSize = STACKSIZE;
 	//musicTaskParams.stack = &musicTaskStack;
 	//displayParams.priority = 1;



    System_printf("Starting tasks\n");
    System_flush();

    //labTask = Task_create(labTaskFxn, &labTaskParams, NULL);
    //if (labTask == NULL) {
    //	System_abort("Task create failed!");
    //}
    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (hMpuPin == NULL)
    {
        System_abort("Sensor pin open failed!");
    }
    sensorTaskVar = Task_create(sensorTask, &labTaskParams, NULL);
    if (sensorTaskVar == NULL)
    {
    	System_abort("Task create failed");
    }

    displayTaskVar = Task_create(displayTask, &displayParams, NULL);
    if(displayTaskVar == NULL)
    {
    	System_abort("displayTask failed");
    }
   //musicTask = Task_create(musicalTask, &musicTaskParams, NULL);
    // 	if (musicTask == NULL) {
     //		System_abort("Task create failed!");
     //	}

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

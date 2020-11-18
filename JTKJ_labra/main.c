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
//#include "kuva.h"
#include "buzzer.h"

#include "wireless/comm_lib.h"
#include "sensors/opt3001.h"
#include "sensors/mpu9250.h"

/* Task */
/* Task stacks */
#define STALESTACK 512
#define STACKSIZE 2048
#define EDGE_VALUES 70
#define MUSICSTACK 1024

Char labTaskStack[STACKSIZE];
//Char defaultStateStack[STALESTACK];
Char displayStack[STACKSIZE];
Char musicTaskStack[MUSICSTACK];
// Char commTaskStack[STACKSIZE];

static PIN_Handle hMpuPin;
static PIN_State MpuPinState;
static PIN_Config MpuPinConfig[] = {
	Board_MPU_POWER  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	PIN_TERMINATE
};

static PIN_Handle buzzer;
static PIN_State buzzerState;


PIN_Config buzzerConfig[] =
{
	// Vili: Tämä rivi on CC2650STK.c:stä
	Board_BUZZER     | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	PIN_TERMINATE
};

static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
	.pinSDA = Board_I2C0_SDA1,
	.pinSCL = Board_I2C0_SCL1
};

enum state{START, READ_SENSOR, UPDATE, NEW_MSG};
enum state myState = START;

enum movement{STILL, LEFT, RIGHT, UP, DOWN};
enum movement move = STILL;

enum game{MENU, GAME};
enum game gameState = MENU;

enum menu{PLAY, MUTE, QUIT};
enum menu menuChoice = PLAY;


/* Task Functions */

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

	while(1)
	{
		if(myState == READ_SENSOR)
		{
			i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
			if (i2cMPU == NULL)
			{
				System_abort("Error Initializing I2CMPU\n");
			}

			mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
			//sprintf(string, "x: %f | y: %f\n", gx, gy);

			if(gx > 80)
			{
				move = UP;
				myState = UPDATE;
			}
			else if(gx < -80)
			{
				move = DOWN;
				myState = UPDATE;
			}
			else if(gy > 80)
			{

				move = RIGHT;
				myState = UPDATE;
			}
			else if(gy < -80)
			{
				move = LEFT;
				myState = UPDATE;
			}
			else
			{
				move = STILL;
			}
			I2C_close(i2cMPU);

		}
		Task_sleep(100000/Clock_tickPeriod);
	}
}

// Vili:
// Tämä taski vie turhaan muistia. displayTask asettaa joka tapauksessa myState = READ_SENSOR
/*Void staleTask(UArg arg0, UArg arg1)
	{

	if(myState == START)
	{
	myState = READ_SENSOR;
	}

	}*/

void setMenuState()
{
	if(menuChoice <= QUIT && move == LEFT && menuChoice != PLAY)
	{
		menuChoice--;
	}
	else if(move == RIGHT && menuChoice != QUIT)
	{
		menuChoice++;
	}
}

Void displayTask(UArg arg0, UArg arg1)
{
	//char disp_messages[10][10] = {"stay", "left", "right", "up", "down"};
	//char str[5];
	System_printf("Started displayTask\n");
	System_flush();

	Display_Params params;
	Display_Params_init(&params);
	params.lineClearMode = DISPLAY_CLEAR_BOTH;
	Display_Handle displayHandle = Display_open(Display_Type_LCD, &params);
	Display_clear(displayHandle); // Vili: Display kannattaa clearata, että siihen ei jää mitään kummittelemaan

	// Vili: Siirsin grlibContextin luomisen alkuun, ettei sitä tehdä joka kerta
	tContext *pContext = DisplayExt_getGrlibContext(displayHandle);

	while(1)
	{
		if(myState == UPDATE && displayHandle && gameState == GAME)
		{
			if (pContext)
			{
				//	sprintf(str, "%s", disp_messages[move]);
				if(move == LEFT)
				{
					Display_print0(displayHandle, 5, 5, "LEFT");
					//GrImageDraw(pContext, &arrowL, 0, 0);
					//GrFlush(pContext);

				}
				else if(move == RIGHT)
				{
					Display_print0(displayHandle, 5, 5, "RIGHT");
					//GrImageDraw(pContext, &arrowR, 0, 0);
					//GrFlush(pContext);

				}
				else if(move == UP)
				{
					Display_print0(displayHandle, 5, 5, "UP");
					//GrImageDraw(pContext, &arrowU, 0, 0);
					//GrFlush(pContext);;

				}
				else if(move == DOWN)
				{
					Display_print0(displayHandle, 5, 5, "DOWN");
					//GrImageDraw(pContext, &arrowD, 0, 0);
					//GrFlush(pContext);
				}
				else
				{
					//GrImageDraw(pContext, &gondola, 0, 0);
					//GrFlush(pContext);;
					//Task_sleep(100000/Clock_tickPeriod);
				}
			}

		}
		else if(myState == UPDATE && displayHandle && gameState == MENU)
		{
			setMenuState();
			if(menuChoice == MUTE)
			{
				Display_print0(displayHandle, 5, 5, "MUTE");
				Task_sleep(1000000/Clock_tickPeriod);
			}
			else if(menuChoice == PLAY)
			{
				Display_print0(displayHandle, 5, 5, "PLAY");
				Task_sleep(1000000/Clock_tickPeriod);
			}
			else if(menuChoice == QUIT)
			{
				Display_print0(displayHandle, 5, 5, "QUIT");
				Task_sleep(1000000/Clock_tickPeriod);
			}
		}
		myState = READ_SENSOR;
		// Vili: Tämä pitää olla tässä että toinen saman prioriteetin taski pääsee pyörimään (musiikki)
		Task_sleep(1000000/Clock_tickPeriod);

	}
}

/*Void menuTask(UArg arg0, UArg arg1)
{
	// TODO: button make button to change state from MENU -> GAME
	System_printf("Started menuTask\n");
	System_flush();
	while(1)
	{
		if(myState == MAIN && gameState == MENU)
		{
			if(menu > 0 && move == RIGHT)
			{
				menu--;
			}
			else if(menu < 2 && move == LEFT)
			{
				menu++;
			}
		}
		Task_sleep(400000/Clock_tickPeriod);
		myState = UPDATE;

	}
}
 */
Void musicTask(UArg arg0, UArg arg1) {
	System_printf("started musicTask\n");
	System_flush();
	while(1) {
		if (gameState == GAME) {
		//if (menuChoice == PLAY) { // Vili: Tällä voi kokeilla
			buzzerOpen(buzzer);
			buzzerSetFrequency(250); // d
			Task_sleep(10000 / Clock_tickPeriod);

			buzzerSetFrequency(400); // f
			Task_sleep(10000 / Clock_tickPeriod);

			buzzerSetFrequency(250); // d
			Task_sleep(10000 / Clock_tickPeriod);

			buzzerSetFrequency(400); // d
			Task_sleep(10000 / Clock_tickPeriod);

			buzzerClose();
			Task_sleep(10000 / Clock_tickPeriod);
		}
		else {
			Task_sleep(10000 / Clock_tickPeriod);
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
	//Task_Handle defaultState;
	//Task_Params defaultStateParams;

	Task_Handle sensorTaskVar;
	Task_Params labTaskParams;

	Task_Handle displayTaskVar;
	Task_Params displayParams;

	Task_Handle musicTaskVar;
	Task_Params musicParams;

	//Task_Handle menuTaskVar;
	//Task_Params menuParams;

	/*
		 Task_Handle commTask;
		 Task_Params commTaskParams;
	 */

	// Initialize board
	Board_initGeneral();
	Board_initI2C();

	buzzer = PIN_open(&buzzerState, buzzerConfig);
	if (buzzer == NULL)
	{
		System_abort("Buzzer pin open failed!");
	}

	// MPU stuff
	hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
	if (hMpuPin == NULL) {
		System_abort("Pin open failed!");
	}

	/* Task */
	Task_Params_init(&labTaskParams);
	labTaskParams.stackSize = STACKSIZE;
	labTaskParams.stack = &labTaskStack;
	labTaskParams.priority = 3;

	/*
	Task_Params_init(&defaultStateParams);
	defaultStateParams.stackSize = STALESTACK;
	defaultStateParams.stack = &defaultStateStack;
	defaultStateParams.priority = 3;
	 */

	Task_Params_init(&displayParams);
	displayParams.stackSize = STACKSIZE;
	displayParams.stack = &displayStack;
	displayParams.priority = 1;

	Task_Params_init(&musicParams);
	musicParams.stackSize = MUSICSTACK;
	musicParams.stack = &musicTaskStack;
	musicParams.priority = 1;

	/*Task_Params_init(&menuParams);
		menuParams.stackSize = MUSICSTACK;
		menuParams.stack = &menuTaskStack;
		menuParams.priority = 2; */

	//Task_Params_init(&commTaskParams);
	//commTaskParams.stackSize = STACKSIZE;
	//commTaskParams.stack = &commTaskStack;
	//commTaskParams.priority=1;

	System_printf("Starting tasks\n");
	System_flush();

	//labTask = Task_create(labTaskFxn, &labTaskParams, NULL);
	//if (labTask == NULL) {
	//	System_abort("Task create failed!");
	//}

	/*
	defaultState = Task_create(staleTask, &defaultStateParams, NULL);
	if(defaultState == NULL)
	{
		System_abort("Task create failed");
	}
	*/

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

	musicTaskVar = Task_create(musicTask, &musicParams, NULL);
	if (musicTaskVar == NULL)
	{
		System_abort("musicTask failed");
	}

	/*menuTaskVar = Task_create(menuTask, &menuParams, NULL);
		if(menuTaskVar == NULL)
		{
		System_abort("menuTask failed");
		}
	 */
	//Init6LoWPAN(); // This function call before use!

	//commTask = Task_create(commTaskFxn, &commTaskParams, NULL);
	//if (commTask == NULL) {
	//	System_abort("Task create failed!");
	//}

	/* Sanity check */
	System_printf("Hello world!\n");
	System_flush();

	/* Start BIOS */
	BIOS_start();

	return (0);
}

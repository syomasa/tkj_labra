/* Authors
 * Mikko Kaasila - 2502425
 * Santeri Hyvärinen - 2590990
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
#include "kuva.h"
#include "buzzer.h"

#include "wireless/comm_lib.h"
#include "sensors/opt3001.h"
#include "sensors/mpu9250.h"

#define DEBUG_MUSIC 1
#define TEST_SEND 0
#define OR ||
/* Task */
/* Task stacks */
#define STALESTACK 512
#define STACKSIZE 2048
#define MUSICSTACK 1024

Char labTaskStack[STACKSIZE];
Char displayStack[STACKSIZE];
Char musicTaskStack[MUSICSTACK];
Char commTaskStack[STACKSIZE];

static PIN_Handle hMpuPin;
static PIN_State MpuPinState;
static PIN_Config MpuPinConfig[] = {
	Board_MPU_POWER  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	PIN_TERMINATE
};

static PIN_Handle buzzer;
static PIN_State buzzerState;

static PIN_Handle buttonHandle;
static PIN_State buttonState;

static PIN_Handle button1Handle;
static PIN_State button1State;

PIN_Config buzzerConfig[] =
{
	// Vili: Tämä rivi on CC2650STK.c:stä
	Board_BUZZER     | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	PIN_TERMINATE
};

PIN_Config buttonConfig[] = {
		Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE, // Hox! TAI-operaatio
		PIN_TERMINATE // Määritys lopetetaan aina tähän vakioon
};

PIN_Config button1Config[] = {
		Board_BUTTON1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE, // Hox! TAI-operaatio
		PIN_TERMINATE
};


static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
	.pinSDA = Board_I2C0_SDA1,
	.pinSCL = Board_I2C0_SCL1
};

enum state{START, READ_SENSOR, UPDATE, NEW_MSG};
enum state myState = UPDATE;

enum movement{STILL=0, LEFT=-2, RIGHT=1, UP=2, DOWN=-3}; // Allows reversing moves using binary negation
enum movement move = STILL;

enum game{MENU, GAME, WIN, LOSE};
enum game gameState = MENU;

enum menu{PLAY, QUIT};
enum menu menuChoice = PLAY;

enum movement moves[32]; // playback up to 32 moves
int move_head = 0; // index of latest move;

int clear = 0;

/* Task Functions */
void add_latest_move(enum movement latest_move)
{
	if(move_head < 32)
	{
		moves[move_head] = latest_move;
		move_head++;
	}
	else
	{
		move_head = 0;
		moves[move_head] = latest_move;
	}
}

void undo_move()
{
	char a[32];
	int rev_last_move = ~moves[move_head-1];
	move = (enum movement) rev_last_move;
	sprintf(a, "Index = %d\nReversed move = %d\n", move_head, rev_last_move);
	System_printf(a);
	System_flush();
	if(move_head < 0)
	{
		move_head = 31;
	}
	else
	{
		move_head--;
	}
	myState = UPDATE;
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
				add_latest_move(move);
				myState = UPDATE;
			}
			else if(gx < -80)
			{
				move = DOWN;
				add_latest_move(move);
				myState = UPDATE;
			}
			else if(gy > 80)
			{

				move = RIGHT;
				add_latest_move(move);
				myState = UPDATE;
			}
			else if(gy < -80)
			{
				move = LEFT;
				add_latest_move(move);
				myState = UPDATE;
			}
			else if(gameState==WIN || gameState==LOSE)
			{
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

void sendMsg(char* msg)
{
	Send6LoWPAN(IEEE80154_SERVER_ADDR, msg, strlen(msg));
	StartReceive6LoWPAN();
	//System_printf(msg);
	//System_flush();
}

void setMenuState(Display_Handle displayHandle,tContext *pContext)
{
	if(move == UP && menuChoice == PLAY)
	{
		Display_clear(displayHandle);
		gameState = GAME;
		GrImageDraw(pContext, &gondola, 0, 0);
		GrFlush(pContext);
		Display_print0(displayHandle, 0, 12, "QUIT");
		Task_sleep(1000000 / Clock_tickPeriod); // Odottaa sekunnin vähentää vahinko liikeitä peliä aloittaessa
		//move = STILL;
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
					//Display_print0(displayHandle, 5, 5, "LEFT");
					sendMsg("event:LEFT");
					GrImageDraw(pContext, &arrowL, 0, 0);
					GrFlush(pContext);
					Task_sleep(1000000/Clock_tickPeriod);
					GrImageDraw(pContext, &gondola, 0, 0);
					GrFlush(pContext);
					Display_print0(displayHandle, 0, 12, "QUIT");

				}
				else if(move == RIGHT)
				{
					//Display_print0(displayHandle, 5, 5, "RIGHT");
					sendMsg("event:RIGHT");
					GrImageDraw(pContext, &arrowR, 0, 0);
					GrFlush(pContext);
					Task_sleep(1000000/Clock_tickPeriod);
					GrImageDraw(pContext, &gondola, 0, 0);
					GrFlush(pContext);
					Display_print0(displayHandle, 0, 12, "QUIT");

				}
				else if(move == UP)
				{
					//Display_print0(displayHandle, 5, 5, "UP");
					sendMsg("event:UP");
					GrImageDraw(pContext, &arrowU, 0, 0);
					GrFlush(pContext);;
					Task_sleep(1000000/Clock_tickPeriod);
					GrImageDraw(pContext, &gondola, 0, 0);
					GrFlush(pContext);
					Display_print0(displayHandle, 0, 12, "QUIT");

				}
				else if(move == DOWN)
				{
					// Display_print0(displayHandle, 5, 5, "DOWN");
					sendMsg("event:DOWN");
					GrImageDraw(pContext, &arrowD, 0, 0);
					GrFlush(pContext);
					Task_sleep(1000000/Clock_tickPeriod);
					GrImageDraw(pContext, &gondola, 0, 0);
					GrFlush(pContext);
					Display_print0(displayHandle, 0, 12, "QUIT");

				}
				else
				{
					Display_clear(displayHandle);
				}
			}

		}
		else if(myState == UPDATE && displayHandle && gameState == MENU)
		{
			if(clear == 1)
			{
				Display_clear(displayHandle);
				clear = 0;
			}
			if(menuChoice == PLAY)
			{
				Display_print0(displayHandle, 3, 6, "^^");
				Display_print0(displayHandle, 5, 5, "PLAY");
			}
			else if(menuChoice == QUIT)
			{
				Display_print0(displayHandle, 5, 5, "<- QUIT");
			}

			setMenuState(displayHandle, pContext);
		}
		else if(myState == UPDATE && displayHandle && gameState == WIN)
		{
			Display_print0(displayHandle, 5, 5, "You won");
			gameState = MENU;
			Task_sleep(3 * 1000000 / Clock_tickPeriod);
			Display_clear(displayHandle);
			continue;
		}
		else if(myState == UPDATE && displayHandle && gameState == LOSE)
		{
			Display_print0(displayHandle, 5, 5, "You lose");
			gameState = MENU;
			Task_sleep(3 * 1000000 / Clock_tickPeriod);
			Display_clear(displayHandle);
			continue;
		}
		myState = READ_SENSOR;
		// Vili: Tämä pitää olla tässä että toinen saman prioriteetin taski pääsee pyörimään (musiikki)
		Task_sleep(1000000/Clock_tickPeriod);

	}
}

/*Void menuTask(UArg arg0, UArg arg1)
{

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
	while(DEBUG_MUSIC) {

		if (gameState == GAME)
		{
		//if (menuChoice == PLAY) { // Vili: Tällä voi kokeilla
			buzzerOpen(buzzer);
			buzzerSetFrequency(750); // d
			Task_sleep(2*100000 / Clock_tickPeriod);
			buzzerClose();
			Task_sleep(100000 / Clock_tickPeriod);

			buzzerOpen(buzzer);
			buzzerSetFrequency(750); // d
			Task_sleep(2*100000 / Clock_tickPeriod);
			buzzerClose();
			Task_sleep(100000 / Clock_tickPeriod);

			buzzerOpen(buzzer);
			buzzerSetFrequency(750);
			Task_sleep(2*100000 / Clock_tickPeriod);

			buzzerSetFrequency(1250);
			Task_sleep(1000000 / Clock_tickPeriod);

			buzzerSetFrequency(1000);
			Task_sleep(2*100000 / Clock_tickPeriod);
			buzzerClose();
			Task_sleep(100000 / Clock_tickPeriod);

			buzzerOpen(buzzer);
			buzzerSetFrequency(1000);
			Task_sleep(2*100000 / Clock_tickPeriod);
			buzzerClose();
			Task_sleep(100000 / Clock_tickPeriod);

			buzzerOpen(buzzer);
			buzzerSetFrequency(1000);

			buzzerSetFrequency(1500);
			Task_sleep(1000000 / Clock_tickPeriod);

			buzzerSetFrequency(1250);
			Task_sleep(2*100000 / Clock_tickPeriod);
			buzzerClose();
			Task_sleep(100000 / Clock_tickPeriod);

			buzzerOpen(buzzer);
			buzzerSetFrequency(1250);
			Task_sleep(2*100000 / Clock_tickPeriod);

			buzzerSetFrequency(1000);
			Task_sleep(2*100000 / Clock_tickPeriod);
			buzzerClose();
			Task_sleep(100000 / Clock_tickPeriod);

			buzzerOpen(buzzer);
			buzzerSetFrequency(1000);
			Task_sleep(2*100000 / Clock_tickPeriod);

			buzzerSetFrequency(750);
			Task_sleep(5*100000 / Clock_tickPeriod);

			buzzerClose();
			Task_sleep(10000 / Clock_tickPeriod);
		}
		else if(gameState == WIN)
		{
			buzzerOpen(buzzer);

			buzzerSetFrequency(750);
			Task_sleep(2*100000 / Clock_tickPeriod);

			buzzerSetFrequency(1000);
			Task_sleep(2*100000 / Clock_tickPeriod);

			buzzerSetFrequency(1250);
			Task_sleep(2*100000 / Clock_tickPeriod);

			buzzerSetFrequency(1000);
			Task_sleep(2*100000 / Clock_tickPeriod);

			buzzerSetFrequency(750);
			Task_sleep(2*100000 / Clock_tickPeriod);

			buzzerClose();
			Task_sleep(10000 / Clock_tickPeriod);
		}
		else if(gameState == LOSE)
		{
			buzzerOpen(buzzer);

			buzzerSetFrequency(500);
			Task_sleep(5*100000 / Clock_tickPeriod);

			buzzerSetFrequency(300);
			Task_sleep(5*100000 / Clock_tickPeriod);

			buzzerSetFrequency(150);
			Task_sleep(5*100000 / Clock_tickPeriod);

			buzzerClose();
			Task_sleep(10000 / Clock_tickPeriod);
		}
		else {
			Task_sleep(10000 / Clock_tickPeriod);
		}
	}
}
/* Communication Task */
Void commTaskFxn(UArg arg0, UArg arg1) {

	// Radio to receive mode
	char msg[16];
	char* ptr_parse;
	//char test_var[64];
	uint16_t sendrAddr;
	System_printf("Started comTask\n");
	System_flush();

	int32_t result = StartReceive6LoWPAN();
	if(result != true) {
		System_abort("Wireless receive mode failed");
	}

	while (1)
	{
		// If true, we have a message
		if (GetRXFlag() == true && gameState == GAME) // gameState == GAME
		{
			memset(msg, 0, 16);
			Receive6LoWPAN(&sendrAddr, msg, 16);

			ptr_parse = strtok(msg, ",");

			if(atoi(ptr_parse) == 427)
			{
				ptr_parse = strtok(NULL, ",");
				int val = strcmp(ptr_parse, "WIN");
				int val2 = strcmp(ptr_parse, "LOST GAME");

				//sprintf(test_var, "Win comparison: %d\nLose comparison: %d\n", val, val2);
				if(strcmp(ptr_parse, "WIN") == 0)
				{
					gameState = WIN;
				}
				else if(strcmp(ptr_parse, "LOST GAME") == 0)
				{
					gameState = LOSE;
				}
				//System_printf(test_var);
				//System_flush();
			}



		}
		// Absolutely NO Task_sleep in this task!!
	}
}

void buttonFxn(PIN_Handle handle, PIN_Id pinId)
{
	if(pinId == Board_BUTTON0)
	{
		System_printf("BUTTON0 pressed\n");
		System_flush();
		gameState = MENU;
		myState = UPDATE;
		clear = 1;
	}
	else if(pinId == Board_BUTTON1)
	{
		undo_move();
	}
}
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

	Task_Handle commTask;
	Task_Params commTaskParams;


	// Initialize board
	Board_initGeneral();
	Board_initI2C();

	buzzer = PIN_open(&buzzerState, buzzerConfig);
	if (buzzer == NULL)
	{
		System_abort("Buzzer pin open failed!");
	}

	buttonHandle = PIN_open(&buttonState, buttonConfig);
	if(!buttonHandle)
	{
		System_abort("Error initializing button pins\n");
	}

	button1Handle = PIN_open(&button1State, button1Config);
	if(!button1Handle)
	{
			System_abort("Error initializing button pins\n");
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
	labTaskParams.priority = 2;


	Task_Params_init(&displayParams);
	displayParams.stackSize = STACKSIZE;
	displayParams.stack = &displayStack;
	displayParams.priority = 2;

	Task_Params_init(&musicParams);
	musicParams.stackSize = MUSICSTACK;
	musicParams.stack = &musicTaskStack;
	musicParams.priority = 2;

	Task_Params_init(&commTaskParams);
	commTaskParams.stackSize = STACKSIZE;
	commTaskParams.stack = &commTaskStack;
	commTaskParams.priority=1;

	System_printf("Starting tasks\n");
	System_flush();

	sensorTaskVar = Task_create(sensorTask, &labTaskParams, NULL);
	if (sensorTaskVar == NULL)
	{
		System_abort("Task create failed");
	}

	Init6LoWPAN(); // This function call before use!

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

	commTask = Task_create(commTaskFxn, &commTaskParams, NULL);
	if (commTask == NULL)
	{
		System_abort("Task create failed!");
	}

	if(PIN_registerIntCb(buttonHandle, &buttonFxn) != 0)
	{
		System_abort("Error registering button callback function");
	}
	if(PIN_registerIntCb(button1Handle, &buttonFxn) != 0)
	{
		System_abort("Error registering button1 callback function");
	}

	/* Sanity check */
	System_printf("Hello world!\n");
	System_flush();

	/* Start BIOS */
	BIOS_start();

	return (0);
}

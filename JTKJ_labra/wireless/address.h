//DESCRIPTION/NOTES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Name:			comm_lib.c
//		Description:	Functions for communication atop 6LoWPAN
// 		Author(s):		Konstantin Mikhaylov, CWC, UOulu & Teemu Leppanen, UBIComp, UOulu
//		Last modified:	2016.10.17
//		Note: 			The commenting style is optimized for automatic documentation generation using DOXYGEN: www.doxygen.org/
//		License:		Refer to Licence.txt file
//						partially based on CC2650 code of Contiki
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IEEE80154_PANID				0x1337
#define IEEE80154_CHANNEL			0x0C

// JTKJ: Alusta laitteen nettiosoite (boksin numero) vakioon
// JTKJ: Give the device a network address (the number in your box)
#define IEEE80154_MY_ADDR			0x0427
#define IEEE80154_SERVER_ADDR 		0x1234

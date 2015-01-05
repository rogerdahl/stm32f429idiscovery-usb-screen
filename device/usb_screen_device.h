// It works in USB FS or USB HS in FS mode.
//
// By default, library works in USB FS mode (for STM32F4-Discovery board).
// If you want to use this on STM32F429-Discovery board, you have to activate USB HS in FS mode.
// Activate this with lines below in your defines.h file:
//
// 	//Activate USB HS in FS mode
// 	#define USE_USB_OTG_HS
//
// Pinout (can not be changed)
//
// 	USB			|STM32F4xx FS mode				|STM32F4xx HS in FS mode	|Notes
// 				|STM32F4-Discovery				|STM32F429-Discovery
//
// 	Data +		PA12							PB15						Data+ for USB, standard and used pin
// 	Data -		PA11							PB14						Data- for USB, standard and used pin
// 	ID			PA10							PB12						ID pin, used on F4 and F429 discovery boards, not needed if you don't like it
//	VBUS		PA9								PB13						VBUS pin, used on F4 and F429 discovery board for activating USB chip.
//																			You have to use VBUS on discovery boards, but on nucleo, it's ok with only Data+ and Data- pins
// Disable necessary pins
//
// USB technically needs only Data+ and Data- pins.
// Also, ID pin can be used, but it is not needed.
//
// Disable ID PIN
//
// If you need pin for something else, where ID is located, you can disable this pin for USB.
// Add lines below in your defines.h file:
//
// 	//Disable ID pin
// 	#define USB_HID_HOST_DISABLE_ID
//
// Disable VBUS PIN
//
// VBUS pin is located on Discovery boards, to activate USB chip on board.
// If you are working with Discovery boards, then you need this pin, otherise USB will not work.
// But if you are working on other application (or Nucleo board), you only need Data+ and Data- pins.
// To disable VBUS pin, add lines below in your defines.h file:
//
// 	//Disable VBUS pin
// 	#define USB_HID_HOST_DISABLE_VBUS

#ifndef TM_USB_HIDDEVICE_H
#define TM_USB_HIDDEVICE_H 100

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "defines.h"

#include  "usbd_screen_class.h"
#include  "usbd_screen_usr.h"
#include  "usbd_screen_desc.h"

typedef enum {
	TM_USB_HIDDEVICE_Status_LibraryNotInitialized,
	TM_USB_HIDDEVICE_Status_Connected,
	TM_USB_HIDDEVICE_Status_Disconnected,
	TM_USB_HIDDEVICE_Status_IdleMode,
	TM_USB_HIDDEVICE_Status_Suspended,
	TM_USB_HIDDEVICE_Status_Resumed,
	TM_USB_HIDDEVICE_Status_Configured,
	TM_USB_HIDDEVICE_Status_Reset
} TM_USB_HIDDEVICE_Status_t;


/**
 * Initialize USB HID Device library for work
 * 
 * This library always returns TM_USB_HIDDEVICE_Status_Disconnected
 */
extern TM_USB_HIDDEVICE_Status_t TM_USB_HIDDEVICE_Init(void);

/**
 * Get USB status
 * 
 * Returns TM_USB_HIDDEVICE_Status_Connected, if library is ready to use with USB,
 * otherwise one of members TM_USB_HIDDEVICE_Status_t is returned
 */
extern TM_USB_HIDDEVICE_Status_t TM_USB_HIDDEVICE_GetStatus(void);

#endif

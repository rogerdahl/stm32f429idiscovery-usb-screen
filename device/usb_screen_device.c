/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "usb_screen_device.h"

extern USB_OTG_CORE_HANDLE USB_OTG_dev;
extern TM_USB_HIDDEVICE_Status_t TM_USB_HIDDEVICE_INT_Status;

extern USBD_Class_cb_TypeDef USBD_myclass_cb;

TM_USB_HIDDEVICE_Status_t TM_USB_HIDDEVICE_Init(void) {
	/* Initialize HID device */
	USBD_Init(&USB_OTG_dev,
	#ifdef USE_USB_OTG_HS 
			USB_OTG_HS_CORE_ID,
	#else            
			USB_OTG_FS_CORE_ID,
	#endif
			&USR_desc, // usbd_screen_desc
			&USBD_myclass_cb, //&USBD_screen_cb, // usbd_screen_class
			&USR_cb); // usbd_screen_usr
	
	TM_USB_HIDDEVICE_INT_Status = TM_USB_HIDDEVICE_Status_Reset;
	
	/* Device not connected */
	return TM_USB_HIDDEVICE_INT_Status;
}

TM_USB_HIDDEVICE_Status_t TM_USB_HIDDEVICE_GetStatus(void) {
	return TM_USB_HIDDEVICE_INT_Status;
}

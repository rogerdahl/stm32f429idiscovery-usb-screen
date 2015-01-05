// Custom USB Class

#include <stddef.h>

#include "usb_core.h"
#include "usb_dcd.h"
#include "usbd_core.h"

#include "tm_stm32f4_ili9341_ltdc.h"

// @note In HS mode and when the DMA is used, all variables and data structures
// dealing with the DMA during the transaction process should be 32-bit aligned.

static uint8_t USBD_SCREEN_Init(void* pdev, uint8_t cfgidx);
static uint8_t USBD_SCREEN_DeInit(void* pdev, uint8_t cfgidx);
static uint8_t USBD_SCREEN_Setup(void* pdev, USB_SETUP_REQ* req);
static uint8_t EP0_SCREEN_TxSent(void* pdev);
static uint8_t EP0_SCREEN_RxReady(void* pdev);
static uint8_t* USBD_SCREEN_GetCfgDesc(uint8_t speed, uint16_t* length);
static uint8_t USBD_SCREEN_DataIn(void* pdev, uint8_t epnum);
static uint8_t USBD_SCREEN_DataOut(void* pdev, uint8_t epnum);
static uint8_t USBD_SCREEN_SOF(void* pdev);
//static uint8_t USBD_SCREEN_IsoINIncomplete(void* pdev);
//static uint8_t USBD_SCREEN_IsoOUTIncomplete(void* pdev);

// Endpoint Address. high bit on = IN, off = OUT. IN = DEVICE TO HOST. OUT = HOST TO DEVICE
#define VIDEO_ENDPOINT_OUT 0x01

// 320 * 240 * 2 = 153,600
// 64 * 512 = 32768
// Width = 320 * 2 = 640
// 153600 / 4 = 38400 (fits in 16 bit USB transfer size)
#define SCREEN_BLOCK_SIZE_BYTES 38400

// The M32F429ZIT6 microcontroller, on which the 32F429IDISCOVERY is based, has
// 2 USB peripherals, one of which supports High Speed (HS, 480 Mbps, 60MB/s).
// The DM00093903.pdf manual states that only Full Speed (FS, 12Mbps, 1.5MB/s)
// is available on the 32F429IDISCOVERY. I'm pretty sure it's because the HS
// support requires an external PHY (an extra chip) while almost everything
// needed for FS is built-in.
//
// This is a confusing because, as far as I can tell, it's the USB peripheral
// that supports HS that is actually used (only it falls back to an FS mode).
// So, we have to define USE_USB_OTG_HS (in the Makefile) to get things working.
// BUT we cannot go above a packet size of 64 bytes, which is max in FS.

#define BULK_PACKET_SIZE 64

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
	#if defined ( __ICCARM__ ) //!< IAR Compiler
		#pragma data_alignment=4
	#endif
#endif
//__ALIGN_BEGIN static uint8_t buf[SCREEN_BLOCK_SIZE_BYTES] __ALIGN_END;

uint16_t current_line;

USBD_Class_cb_TypeDef USBD_myclass_cb = {
  USBD_SCREEN_Init,
  USBD_SCREEN_DeInit,
	// Control Endpoints
  USBD_SCREEN_Setup,
  EP0_SCREEN_TxSent,
  EP0_SCREEN_RxReady,
	// Class Specific Endpoints
  USBD_SCREEN_DataIn, // DataIn. In = DEVICE TO HOST
  USBD_SCREEN_DataOut, // DataOut. OUT = HOST TO DEVICE
  NULL, // USBD_SCREEN_SOF, // probably wasn't handling SOF correctly. What happens after SOF? Why do I get SOF?
  NULL, // USBD_SCREEN_IsoINIncomplete,
  NULL, // USBD_SCREEN_IsoOUTIncomplete,
  USBD_SCREEN_GetCfgDesc,
#ifdef USB_OTG_HS_CORE
  USBD_SCREEN_GetCfgDesc, // use same config as per FS
#endif
};

//#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
//	#if defined ( __ICCARM__ ) //!< IAR Compiler
//		#pragma data_alignment=4
//	#endif
//#endif
//__ALIGN_BEGIN static uint32_t USBD_SCREEN_AltSet __ALIGN_END = 0;
//
//#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
//	#if defined ( __ICCARM__ ) //!< IAR Compiler
//		#pragma data_alignment=4
//	#endif
//#endif
//__ALIGN_BEGIN static uint32_t USBD_SCREEN_Protocol __ALIGN_END = 0;
//
//#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
//	#if defined ( __ICCARM__ ) //!< IAR Compiler
//		#pragma data_alignment=4
//	#endif
//#endif
//__ALIGN_BEGIN static uint32_t USBD_SCREEN_IdleState __ALIGN_END = 0;

// Compiler diags.

#ifdef DIAG

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
	#pragma message "USB_OTG_HS_INTERNAL_DMA_ENABLED"
#endif

#ifdef USE_ULPI_PHY
	#pragma message "USE_ULPI_PHY"
#endif

#ifdef USB_OTG_ULPI_PHY_ENABLED
  #pragma message "USB_OTG_ULPI_PHY_ENABLED"
#endif

#ifdef USE_EMBEDDED_PHY
  #pragma message "USE_EMBEDDED_PHY"
#endif

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #pragma message "USB_OTG_HS_INTERNAL_DMA_ENABLED"
#endif

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
  #pragma message "USB_OTG_HS_DEDICATED_EP1_ENABLED"
#endif

#ifdef USB_OTG_HS_LOW_PWR_MGMT_SUPPORT
  #pragma message "USB_OTG_HS_LOW_PWR_MGMT_SUPPORT"
#endif

#endif

// USB descriptors.
// http://www.beyondlogic.org/usbnutshell/usb5.shtml#ConfigurationDescriptors

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
	#if defined ( __ICCARM__ ) //!< IAR Compiler
		#pragma data_alignment=4
	#endif
#endif
__ALIGN_BEGIN static uint8_t USBD_SCREEN_CfgDesc[] __ALIGN_END = {
	// Configuration Descriptor
  0x09, // bLength: Configuration Descriptor size
  0x02, // bDescriptorType: 2 = Configuration
  0x19, 0x00, // wTotalLength: Bytes returned, 9 9 7 = 0x19
  0x01, // bNumInterfaces: 1 interface
  0x01, // bConfigurationValue: Configuration value
  0x00, // iConfiguration: Index of string descriptor describing the configuration
  0xE0, // bmAttributes: bus powered and Support Remote Wake-up
  0x32, // MaxPower 100 mA: this current is used for detecting Vbus
	// Interface Descriptor
  0x09, // bLength: Interface Descriptor size
  0x04, // bDescriptorType: 4 = Interface descriptor type
  0x00, // bInterfaceNumber: Number of Interface
  0x00, // bAlternateSetting: Alternate setting
  0x01, // bNumEndpoints
  0xFF, // bInterfaceClass: CUSTOM
  0x00, // bInterfaceSubClass : CUSTOM
  0x00, // nInterfaceProtocol : CUSTOM
  0x00, // iInterface: Index of string descriptor
	// Endpoint Descriptor
  0x07, // bLength: Endpoint Descriptor size
  0x05, // bDescriptorType: 5 = Endpoint
  VIDEO_ENDPOINT_OUT, //bEndpointAddress
  0x02, // bmAttributes: bitfield. xxxxxx10 = Bulk, x = reserved bits (for bulk)
	0x40, 0x00, // wMaxPacketSize BULK_PACKET_SIZE=64
  0x00, // bInterval: Ignored for bulk. Isochronous must equal 1
};

// @brief USBD_SCREEN_Init
// Initialize the SCREEN interface
// @param pdev: device instance
// @param cfgidx: Configuration index
// @retval status
static uint8_t USBD_SCREEN_Init(void* pdev, uint8_t cfgidx)
{
  TM_ILI9341_Puts(0, 9 * TM_Font_16x26.FontHeight, "Init", &TM_Font_16x26, 0, 0xffff);
  DCD_EP_Open(pdev, VIDEO_ENDPOINT_OUT, BULK_PACKET_SIZE, USB_OTG_EP_BULK); // USB_OTG_EP_ISOC   USB_OTG_EP_INT
  return USBD_OK;
}

static uint8_t USBD_SCREEN_DeInit(void* pdev, uint8_t cfgidx)
{
  TM_ILI9341_Puts(0, 9 * TM_Font_16x26.FontHeight, "DeInit", &TM_Font_16x26, 0, 0xffff);
  DCD_EP_Close(pdev, VIDEO_ENDPOINT_OUT);
  return USBD_OK;
}

//
// Control Endpoints
//

// @brief USBD_SCREEN_Setup
// Handle the SCREEN specific requests
// @param pdev: instance
// @param req: usb requests
// @retval status
static uint8_t USBD_SCREEN_Setup(void* pdev, USB_SETUP_REQ* req)
{
	//typedef struct usb_setup_req
	//{
	//	uint8_t bmRequest;
	//	uint8_t bRequest;
	//	uint16_t wValue;
	//	uint16_t wIndex;
	//	uint16_t wLength;
	//} USB_SETUP_REQ;
	current_line = req->wValue;
  //TM_ILI9341_Puts(0, 8 * TM_Font_16x26.FontHeight, "Setup", &TM_Font_16x26, 0, 0xffff);
  DCD_EP_Flush(pdev, VIDEO_ENDPOINT_OUT);
	//////DCD_EP_PrepareRx(pdev, VIDEO_ENDPOINT_OUT, buf, SCREEN_BLOCK_SIZE_BYTES); // works
	DCD_EP_PrepareRx(pdev, VIDEO_ENDPOINT_OUT, ((uint8_t*)0xd0000000) + req->wValue * SCREEN_BLOCK_SIZE_BYTES, SCREEN_BLOCK_SIZE_BYTES);
	
  return USBD_OK;
}

static uint8_t EP0_SCREEN_TxSent(void* pdev)
{
  TM_ILI9341_Puts(0, 0 * TM_Font_16x26.FontHeight, "EP0_TxSent", &TM_Font_16x26, 0, 0xffff);
  return USBD_OK;
}

static uint8_t EP0_SCREEN_RxReady(void* pdev)
{
  TM_ILI9341_Puts(0, 1 * TM_Font_16x26.FontHeight, "EP0_RxReady", &TM_Font_16x26, 0, 0xffff);
  return USBD_OK;
}

//
// Class Specific Endpoints
//

// @brief USBD_SCREEN_DataIn
// handle data IN Stage
// @param pdev: device instance
// @param epnum: endpoint index
// @retval status
static uint8_t USBD_SCREEN_DataIn(void* pdev, uint8_t epnum)
{
  TM_ILI9341_Puts(0, 3 * TM_Font_16x26.FontHeight, "DataIn", &TM_Font_16x26, 0, 0xffff);
  return USBD_OK;
}


// @brief USBD_SCREEN_DataOut
// handle data OUT Stage
// @param pdev: device instance
// @param epnum: endpoint index
// @retval status
static uint8_t USBD_SCREEN_DataOut(void* pdev, uint8_t epnum)
{
  //TM_ILI9341_Puts(0, 4 * TM_Font_16x26.FontHeight, "DataOut", &TM_Font_16x26, 0, 0xffff);
	//TM_ILI9341_Puts(0, 10 * TM_Font_16x26.FontHeight, (char*)buf + 2000, &TM_Font_16x26, 0, 0xffff);

	//for (int x = 0; x < 320; ++x) {
	//	//TM_ILI9341_DrawPixel(current_line, x, ((uint16_t*)buf)[x]);
	//	*((uint16_t*)0xD0000000 + current_line * (320 / 2) + x * 2) = ((uint16_t*)buf)[x];
	//}
	

	//static int total = 0;
	//uint16_t received_bytes = ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].xfer_count;
	//total += received_bytes;
	//if (total >= SCREEN_BLOCK_SIZE_BYTES) {
	//	total = 0;
	//}
	
	// Ensure that the FIFO is empty before a new transfer, this condition could
	// be caused by a new transfer before the end of the previous transfer.
	//
	// Having this here causes things to stall with a neverending timeout after
	// a few seconds.
  //DCD_EP_Flush(pdev, VIDEO_ENDPOINT_OUT);

  return USBD_OK;
}

// @brief USBD_SCREEN_GetCfgDesc
// return configuration descriptor
// @param speed : current device speed
// @param length : pointer data length
// @retval pointer to descriptor buffer
static uint8_t* USBD_SCREEN_GetCfgDesc(uint8_t speed, uint16_t* length)
{
  TM_ILI9341_Puts(0, 2 * TM_Font_16x26.FontHeight, "GetCfgDesc", &TM_Font_16x26, 0, 0xffff);
  *length = sizeof(USBD_SCREEN_CfgDesc);
  return USBD_SCREEN_CfgDesc;
}


//static uint8_t USBD_SCREEN_DataOut(void* pdev, uint8_t epnum)
//{      
//  uint16_t USB_Rx_Cnt,i;
//  
//  /* Get the received data buffer and update the counter */
//  USB_Rx_Cnt = ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].xfer_count;
//  /* USB data will be immediately processed, this allow next USB traffic being
//     NAKed till the end of the application Xfer */
//  /* Prepare Out endpoint to receive next packet */
//  DCD_EP_PrepareRx(pdev,
//                   HID_OUT_EP,
//                   (uint8_t*)(USB_Rx_Buffer),
//                   HID_OUT_PACKET);
//
//  return USBD_OK;
//}


/**
* @brief USBD_SOF
* Handle SOF event
* @param pdev: device instance
* @retval status
*/

static uint8_t USBD_SCREEN_SOF(void* pdev)
{
  TM_ILI9341_Puts(0, 5 * TM_Font_16x26.FontHeight, "SOF", &TM_Font_16x26, 0, 0xffff);
	// This crashes.
	//if(((USB_OTG_CORE_HANDLE*)pdev)->dev.class_cb->SOF) {
	// ((USB_OTG_CORE_HANDLE*)pdev)->dev.class_cb->SOF(((USB_OTG_CORE_HANDLE*)pdev));
	//}
  return USBD_OK;
}


/**
* @brief USBD_IsoINIncomplete
* Handle iso in incomplete event
* @param pdev: device instance
* @retval status
*/
static uint8_t USBD_SCREEN_IsoINIncomplete(void* pdev)
{
  TM_ILI9341_Puts(0, 6 * TM_Font_16x26.FontHeight, "IsoINInc", &TM_Font_16x26, 0, 0xffff);
  ((USB_OTG_CORE_HANDLE*)pdev)->dev.class_cb->IsoINIncomplete(((USB_OTG_CORE_HANDLE*)pdev));
  return USBD_OK;
}

/**
* @brief USBD_IsoOUTIncomplete
* Handle iso out incomplete event
* @param pdev: device instance
* @retval status
*/
static uint8_t USBD_SCREEN_IsoOUTIncomplete(void* pdev)
{
  TM_ILI9341_Puts(0, 7 * TM_Font_16x26.FontHeight, "IsoOUTInc", &TM_Font_16x26, 0, 0xffff);
  //((USB_OTG_CORE_HANDLE*)pdev)->dev.class_cb->IsoOUTIncomplete((USB_OTG_CORE_HANDLE*)pdev);
  return USBD_OK;
}

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

//#include "stm32f4xx_gpio.h"
//#include "stm32f4xx_adc.h"
//#include "stm32f4xx_rcc.h"
//#include "stm32f4xx_rtc.h"
//#include "stm32f4xx_exti.h"
//#include "stm32f4xx_pwr.h"
//#include "stm32f4xx_syscfg.h"
//#include "stm32f4xx_dbgmcu.h"

#include "tm_stm32f4_ili9341_ltdc.h"
#include "usb_screen_device.h"


#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_timer_properties.h"
#include "defines.h"

void green_on(void) {
	GPIO_SetBits(GPIOG, GPIO_Pin_13);
}

void green_off(void) {
	GPIO_ResetBits(GPIOG, GPIO_Pin_13);
}

void red_on(void) {
	GPIO_SetBits(GPIOG, GPIO_Pin_14);
}

void red_off(void) {
	GPIO_ResetBits(GPIOG, GPIO_Pin_14);
}

void print_usb_status(void);

void rcc_set_frequency();


volatile int c = 0;
void d(void) {
	for (c = 0; c < 1000000; ++c);
}


int main(void)
{
	/* Initialize system */
	//SystemInit();

	rcc_set_frequency();
	
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_AHB1PeriphClockCmd(ILI9341_WRX_CLK |
												 ILI9341_CS_CLK |
												 ILI9341_RST_CLK |
												 RCC_AHB1Periph_GPIOA |
												 RCC_AHB1Periph_GPIOB |
												 RCC_AHB1Periph_GPIOC |
												 RCC_AHB1Periph_GPIOD |
												 RCC_AHB1Periph_GPIOF |
												 RCC_AHB1Periph_GPIOG |
												 RCC_AHB1Periph_OTG_HS, ENABLE);

	GPIO_InitTypeDef GPIO_InitDef;

	GPIO_InitDef.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	//Initialize pins
	GPIO_Init(GPIOG, &GPIO_InitDef);
		
	/* Initialize leds */
	TM_DISCO_LedInit();
	
	/* Initialize button */
	TM_DISCO_ButtonInit();
	
	/* Initialize delay */
	TM_DELAY_Init();
	
		
	/* Initialize USB HID Device */
	TM_USB_HIDDEVICE_Init();
	
	TM_ILI9341_Init();
	TM_ILI9341_InitPins();
	TM_LCD9341_InitLTDC();
	TM_ILI9341_InitLayers();
	TM_ILI9341_InitLCD();
	
	//TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);
	
	green_on();
	red_off();

	//extern uint8_t buf[8];
		
	while (1) {
		//print_usb_status();

		//for (int x = 0; x < 200; ++x) {
		//	for (int y = 0; y < 200; ++y) {
		//			TM_ILI9341_DrawPixel(x, y, c++);
		//	}
		//	TM_ILI9341_ScrollDown(1, 0);
		//}
		
		//d();
		//green_on();
		//d();		
		//green_off();

		//red_off();
		//red_on();

		//buf[7] = '\0';
		//buf[0] = '-';
		//TM_ILI9341_Puts(0, 10 * TM_Font_16x26.FontHeight, buf, &TM_Font_16x26, 0, 0xffff);

		if (TM_DISCO_ButtonPressed()) {
			red_on();
		}
		else {
			red_off();
		}
	}
}

//void OTG_FS_WKUP_IRQHandler(void) {
//	green_on();
//}
//
//void OTG_HS_IRQHandler(void) {
//	green_on();
//}

void print_usb_status(void) {
	char* status;
	TM_USB_HIDDEVICE_Status_t s = TM_USB_HIDDEVICE_GetStatus();
	if (s == TM_USB_HIDDEVICE_Status_LibraryNotInitialized) {
		status = "uninitialized ";
	}
	else if (s == TM_USB_HIDDEVICE_Status_Connected) {
		status = "connected     ";
		//green_on();
	}
	else if (s == TM_USB_HIDDEVICE_Status_Disconnected) {
		status = "disconnected  ";
		red_on();
	}
	else if (s == TM_USB_HIDDEVICE_Status_IdleMode) {
		status = "idle          ";
	}
	else if (s == TM_USB_HIDDEVICE_Status_Suspended) {
		status = "suspended     ";
	}
	else if (s == TM_USB_HIDDEVICE_Status_Resumed) {
		status = "resumed       ";
	}
	else if (s == TM_USB_HIDDEVICE_Status_Configured) {
		status = "configured    ";
	}
	else if (s == TM_USB_HIDDEVICE_Status_Reset) {
		status = "reset         ";
	}
	else {
		status = "UNKNOWN       ";
	}
	TM_ILI9341_Puts(0, 0 * TM_Font_16x26.FontHeight, status, &TM_Font_16x26, 0, 0xffff);
}


//int main(void) {
//	uint8_t already = 0;
//	
//	/* Set structs for all examples */
//	TM_USB_HIDDEVICE_Keyboard_t Keyboard;
//	TM_USB_HIDDEVICE_Gamepad_t Gamepad1, Gamepad2;
//	TM_USB_HIDDEVICE_Mouse_t Mouse;
//	
//	/* Initialize system */
//	SystemInit();
//	
//	/* Initialize leds */
//	TM_DISCO_LedInit();
//	
//	/* Initialize button */
//	TM_DISCO_ButtonInit();
//	
//	/* Initialize delay */
//	TM_DELAY_Init();
//	
//	/* Initialize USB HID Device */
//	TM_USB_HIDDEVICE_Init();
//	
//	/* Set default values for mouse struct */
//	TM_USB_HIDDEVICE_MouseStructInit(&Mouse);
//	/* Set default values for keyboard struct */
//	TM_USB_HIDDEVICE_KeyboardStructInit(&Keyboard);
//	/* Set default values for gamepad structs */
//	TM_USB_HIDDEVICE_GamepadStructInit(&Gamepad1);
//	TM_USB_HIDDEVICE_GamepadStructInit(&Gamepad2);
//
//	
//	red_on();
//	
//	while (1) {		  
//		/* If we are connected and drivers are OK */
//		if (TM_USB_HIDDEVICE_GetStatus() == TM_USB_HIDDEVICE_Status_Connected) {
//			/* Turn on green LED */
//			TM_DISCO_LedOn(LED_GREEN);
//			
///* Simple sketch start */	
//			
//			/* If you pressed button right now and was not already pressed */
//			if (TM_DISCO_ButtonPressed() && already == 0) { /* Button on press */
//				green_on();
//				already = 1;
//				
//				/* Set pressed keys = WIN + R */
//				Keyboard.L_GUI = TM_USB_HIDDEVICE_Button_Pressed;	/* Win button */
//				Keyboard.Key1 = 0x15; 								/* R */
//				/* Result = "Run" command */
//				
//				/* Send keyboard report */
//				TM_USB_HIDDEVICE_KeyboardSend(&Keyboard);
//			} else if (!TM_DISCO_ButtonPressed() && already == 1) { /* Button on release */
//				already = 0;
//				
//				/* Release all buttons */
//				Keyboard.L_GUI = TM_USB_HIDDEVICE_Button_Released;	/* No button */
//				Keyboard.Key1 = 0x00; 								/* No key */
//				/* Result = Released everything */
//				
//				/* Send keyboard report */
//				TM_USB_HIDDEVICE_KeyboardSend(&Keyboard);
//			}
//			
///* Simple sketch end */
//			
//		} else {
//			/* Turn off green LED */
//			TM_DISCO_LedOff(LED_GREEN);
//		}
//	}
//}
 
void rcc_set_frequency()
{
	// Set to default config. 168MHz core and 48MHz USB.
	// http://waijung.aimagin.com/index.htm?block_diagrams.htm	
	uint32_t PLL_M = 8;
	uint32_t PLL_N = 336;
	uint32_t PLL_Q = 7;
	uint32_t PLL_P = 2;
	
  RCC_DeInit();
 
	/* Enable HSE osscilator */
	RCC_HSEConfig(RCC_HSE_ON);
	
	if (RCC_WaitForHSEStartUp() == ERROR) {
		return;
	}
	
	/* Configure PLL clock M, N, P, and Q dividers */
	RCC_PLLConfig(RCC_PLLSource_HSE, PLL_M, PLL_N, PLL_P, PLL_Q);
	
	/* Enable PLL clock */
	RCC_PLLCmd(ENABLE);
	
	/* Wait until PLL clock is stable */
	while ((RCC->CR & RCC_CR_PLLRDY) == 0);
	
	/* Set PLL_CLK as system clock source SYSCLK */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	/* Set AHB clock divider */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	
	/* Set APBx clock dividers */
	RCC_PCLK1Config(RCC_HCLK_Div4); /* 42MHz */
	RCC_PCLK2Config(RCC_HCLK_Div2); /* 84MHz */
	
	/* Update SystemCoreClock variable */
	SystemCoreClockUpdate();
}

//enum sysclk_freq {
//    SYSCLK_42_MHZ=0,
//    SYSCLK_84_MHZ,
//    SYSCLK_168_MHZ,
//    SYSCLK_200_MHZ,
//    SYSCLK_240_MHZ,
//};
// 
//void rcc_set_frequency(enum sysclk_freq freq);
//
//void rcc_set_frequency(enum sysclk_freq freq)
//{
//    int freqs[]   = {42, 84, 168, 200, 240};
// 
//    /* USB freqs: 42MHz, 42Mhz, 48MHz, 50MHz, 48MHz */
//    int pll_div[] = {2, 4, 7, 10, 10};
// 
//    /* PLL_VCO = (HSE_VALUE / PLL_M) * PLL_N */
//    /* SYSCLK = PLL_VCO / PLL_P */
//    /* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
//    uint32_t PLL_P = 2;
//    uint32_t PLL_N = freqs[freq] * 2;
//    uint32_t PLL_M = (HSE_VALUE/1000000);
//    uint32_t PLL_Q = pll_div[freq];
// 
//    RCC_DeInit();
// 
//    /* Enable HSE osscilator */
//    RCC_HSEConfig(RCC_HSE_ON);
// 
//    if (RCC_WaitForHSEStartUp() == ERROR) {
//        return;
//    }
// 
//    /* Configure PLL clock M, N, P, and Q dividers */
//    RCC_PLLConfig(RCC_PLLSource_HSE, PLL_M, PLL_N, PLL_P, PLL_Q);
// 
//    /* Enable PLL clock */
//    RCC_PLLCmd(ENABLE);
// 
//    /* Wait until PLL clock is stable */
//    while ((RCC->CR & RCC_CR_PLLRDY) == 0);
// 
//    /* Set PLL_CLK as system clock source SYSCLK */
//    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
// 
//    /* Set AHB clock divider */
//    RCC_HCLKConfig(RCC_SYSCLK_Div1);
// 
//    /* Set APBx clock dividers */
//    switch (freq) {
//        /* Max freq APB1: 42MHz APB2: 84MHz */
//        case SYSCLK_42_MHZ:
//            RCC_PCLK1Config(RCC_HCLK_Div1); /* 42MHz */
//            RCC_PCLK2Config(RCC_HCLK_Div1); /* 42MHz */
//            break;
//        case SYSCLK_84_MHZ:
//            RCC_PCLK1Config(RCC_HCLK_Div2); /* 42MHz */
//            RCC_PCLK2Config(RCC_HCLK_Div1); /* 84MHz */
//            break;
//        case SYSCLK_168_MHZ:
//            RCC_PCLK1Config(RCC_HCLK_Div4); /* 42MHz */
//            RCC_PCLK2Config(RCC_HCLK_Div2); /* 84MHz */
//            break;
//        case SYSCLK_200_MHZ:
//            RCC_PCLK1Config(RCC_HCLK_Div4); /* 50MHz */
//            RCC_PCLK2Config(RCC_HCLK_Div2); /* 100MHz */
//            break;
//        case SYSCLK_240_MHZ:
//            RCC_PCLK1Config(RCC_HCLK_Div4); /* 60MHz */
//            RCC_PCLK2Config(RCC_HCLK_Div2); /* 120MHz */
//            break;
//    }
// 
//    /* Update SystemCoreClock variable */
//    SystemCoreClockUpdate();
//}

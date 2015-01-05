# STM32F4 firmware library root directory
STM_COMMON=./STM32F4xx_DSP_StdPeriph_Lib_V1.4.0

# stlink folder directory (for "make burn")
STLINK=/home/dahl/bin/stlink

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME=usb_display

# Compiler

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

# Sources, project

SRCS= main.c

# USB

SRCS += usb_screen_device.c
SRCS += usbd_screen_class.c
SRCS += usbd_screen_desc.c
SRCS += usbd_screen_usr.c
SRCS += usb_bsp.c
SRCS += usbd_req.c

# Misc libraries from Tilen Majerle

SRCS += tm_stm32f4_ili9341_ltdc.c
SRCS += tm_stm32f4_fonts.c
SRCS += tm_stm32f4_sdram.c
SRCS += tm_stm32f4_spi.c
SRCS += tm_stm32f4_disco.c
SRCS += tm_stm32f4_delay.c
SRCS += tm_stm32f4_timer_properties.c

# Sources, STM32F4 firmware library

#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_aes.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_des.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_tdes.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dcmi.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash_ramfunc.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_md5.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_sha1.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c
#SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sai.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c
SRCS += $(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c

# Sources, ARM CMSIS library

SRCS += $(STM_COMMON)/Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c

# Startup file.

SRCS += ./STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f429_439xx.s

# USB library from ST

#SRCS += ./usb_screen_device/usb_bsp.c
SRCS += ./usb_screen_device/usb_core.c
SRCS += ./usb_screen_device/usb_dcd.c
SRCS += ./usb_screen_device/usb_dcd_int.c
SRCS += ./usb_screen_device/usbd_core.c
#SRCS += ./usb_screen_device/usbd_desc.c
#SRCS += ./usb_screen_device/usbd_hid_core.c
SRCS += ./usb_screen_device/usbd_ioreq.c
#SRCS += ./usb_screen_device/usbd_usr.c

# USB library from Tilen Majerle
#SRCS += ./usb_screen_device.c

#
# Compilation
#

CFLAGS  = -g -O2 -Wall

CFLAGS+= -DUSE_STDPERIPH_DRIVER -DSTM32F4XX -DHSE_VALUE=8000000 -DSTM32F429_439xx

# This flag is important not only to set high speed USB but to select the correct
# pins. HS = STM32F429 Discovery, FS=STM32F4 Discovery.
CFLAGS+= -DUSE_USB_OTG_HS

#CFLAGS+= -DUSE_ULPI_PHY
#CFLAGS+= -DUSE_EMBEDDED_PHY
#CFLAGS+= -DUSB_OTG_HS_INTERNAL_DMA_ENABLED

#CFLAGS += -D__GNUC__

#CFLAGS+= -Wno-pointer-sign
CFLAGS+= -Wno-strict-aliasing
CFLAGS+= -Wno-unused-variable
CFLAGS+= -Wno-unused-function

# Linker script

CFLAGS += -T./stm32_flash.ld
#CFLAGS += -T./stm32_sram.ld # didn't work. just an experiment.

# Fix "undefined reference to `_exit'"
CFLAGS += --specs=nosys.specs

CFLAGS += -std=c99 -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork -lm -mfloat-abi=hard -mfpu=fpv4-sp-d16

# Include search directories

CFLAGS += -I.
CFLAGS += -I$(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/inc
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Device/ST/STM32F4xx/Include/
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include/
# USB
CFLAGS += -I./usb_screen_device/

#
# Targets
#

OBJS = $(SRCS:.c=.o)

.PHONY: proj

all: proj

proj: $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ 
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

# Flash
burn: proj
	$(STLINK)/st-flash write $(PROJ_NAME).bin 0x08000000

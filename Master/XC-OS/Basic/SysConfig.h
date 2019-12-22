#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#define XC_VERSION_SW               "v1.0"
#define XC_VERSION_HW               "v1.0"
#define XC_OS_NAME                  "XC-OS"
#define XC_CPU_NAME                 "STM32F405RGT6"
#define XC_CPU_RATE_MHZ             (F_CPU/1000000)
#define XC_KB(b)                    ((b)*1024)
#define XC_ROM_SIZE                 (XC_KB(1024))
#define XC_RAM_SIZE                 (XC_KB(192))
#define XC_BATTERY_VOLTAGE          (3.7)
#define XC_BATTERY_CAPACITY_MAH     (2000)

#define NULL_PIN PD0

#define Motor_Pin           PA0
#define SD_CS_Pin           PA1
#define SD_CD_Pin           PB0
#define Speaker_Pin         PA4
#define AudioMute_Pin       PB1
#define CHG_KEY_Pin         PB2
#define Touch_SDA_Pin       PB3
#define Touch_SCL_Pin       PD2
#define Touch_INT_Pin       PA8

#define TFT_PORT_Pin PC0
#define TFT_RST_Pin  PA15
#define TFT_CS_Pin   PB12
#define TFT_RS_Pin   PB13
#define TFT_RW_Pin   PB14
#define TFT_RD_Pin   PB15
#define TFT_LED_Pin  PB9

#define NRF_MOSI_Pin PB7
#define NRF_MISO_Pin PB8
#define NRF_SCK_Pin  PB6
#define NRF_CE_Pin   PB4
#define NRF_CSN_Pin  PB5
#define NRF_IRQ_Pin  -1
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

#define XC_TIM_WAVPLAYER    TIM8
#define XC_USE_LUA          1

#endif

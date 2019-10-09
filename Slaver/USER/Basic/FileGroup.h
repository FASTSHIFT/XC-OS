#ifndef __FILEGROUP_H
#define	__FILEGROUP_H

/** 
  *@Illustrate: A Cross-platform migration Project | Arduino for STM32F0xx 
  *@Author: FASTSHIFT
  *@Website: https://github.com/FASTSHIFT/Arduino-For-Keil
  */

//************* Arduino Library **************//
#include "Arduino.h"
#include "HardwareSerial.h"
#include "Stream.h"
#include "SPI.h"
#include "Tone.h"
#include "WString.h"
#include "Wire.h"
#include "WMath.h"

//************* STM32 Core Library **************//
#include "adc.h"
#include "delay.h"
#include "exti.h"
#include "gpio.h"
#include "pwm.h"
#include "timer.h"

void Init_BaseClocks(bool internal);
void Init_IWDG(uint8_t prescaler, uint16_t reload);

#endif

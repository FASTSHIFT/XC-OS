# A Cross-platform migration Project @FASTSHIFT
# Arduino for STM32F0xx

# https://github.com/FASTSHIFT/Arduino-For-Keil

//*********************************UPGRADE LOGS************************************//

//Finish in 2018.7.29  V_1.0 ����STM32F0xx��־�����(1.0.1��)����ֲ��ȫ����ArduinoAPI
//Upgrade   2018.8.14  V_1.1 ����USART��ش��룬�޸�Tone��ֻռ��һ����ʱ��(��ʹ��toneSetTimer()�����л�),time_exti����Ϊtimer
//Upgrade   2018.8.17  V_1.2 ��Arduino.h����˸���������GPIO�Ĵ��������������������������ļ�����
//Upgrade   2018.8.20  V_1.3 ���WCharacter.h��֧��Arduino�й�Characters�ĺ��������mcu_type.h��ʹArduino.h���Կ�MCUʹ��
//Upgrade   2018.10.9  V_1.4 ���ע�ͣ�����Tone��HardwareSerial��ȥ��usart.c usart.h
//Upgrade   2018.11.21 V_1.5 ����Arduino.c
//Upgrade   2019.2.18  V_1.6 �������,���õļ�����
//Upgrade   2019.3.22  V_1.7 ΪSysTick��������ж����ȼ�����ֹ�������жϴ��(�ǳ���Ҫ!)
//Upgrade   2019.6.3   V_1.8 pwm.c�⽫Ƶ���趨ֵ��16λ��Ϊ32λ������������Ϸ���Χ�жϣ�ARDUINO�궨���Arduino.hת��ȫ�ֺ궨��
//Upgrade   2019.6.18  V_1.9 PA4�ѿ������PWM
//Upgrade   2019.7.12  V_2.0 Arduino.h�����yield()��digitalPinToInterrupt(Pin)���壬����analogRead_DMA()�Ϸ������жϷ�ʽ
//Upgrade   2019.10.3  V_2.1 ͬ��_Keilduino(STM32F10x) V_7.0���ִ��룬��ʱȥ��ADC DMA����STM32F030F4P6����1ӳ�䵽PA9��PA10�����TIM6��ʱ�ж�
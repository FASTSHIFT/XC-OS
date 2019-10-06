#include "FileGroup.h"
#include "CommonMacro.h"

#define LED_Pin PB1

void setup()
{
//    pinMode(LED_Pin, OUTPUT);
    Serial.begin(115200);
    Serial.setTimeout(20);
    
//    while(!Serial.available())
//    {
//        __IntervalExecute(togglePin(LED_Pin), 2000);
//    }
    pinMode(LED_Pin, PWM);
    analogWrite(LED_Pin, 500);
}

char str[100];
int cnt = 0;
int val;

void loop()
{
    if(Serial.available() > 100)
    {
        Serial.flush();
    }
    if(Serial.available())
    {
        char c = Serial.read();
        if(c == 'E')
        {
            val = String(str+1).toInt();
            analogWrite(LED_Pin, val);
            memset(str, 0, sizeof(str));
            cnt=0;
        }
        else
        {
            str[cnt] = c;
            cnt++;
        }
//        String recv = Serial.readString();
//        strcpy(r,recv.c_str());
//        if(recv.startsWith("param:"))
//        {
//            int param = recv.toInt();
//            analogWrite(LED_Pin, param);
//        }
//        Serial.flush();
    }
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    Delay_Init();
    ADCx_Init(ADC1);
    setup();
    for(;;)loop();
}

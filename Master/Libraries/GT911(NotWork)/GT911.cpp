#include "GT911.h"
#include "Wire.h"

#define TOUCH_SDA_Pin PB3
#define TOUCH_SCL_Pin PD2
static TwoWire touchWire(TOUCH_SCL_Pin, TOUCH_SDA_Pin, SOFT_STANDARD);

#define RST_CTRL_SET    digitalWrite(RST_Pin,HIGH)//GT911 RESET pin out high or low
#define RST_CTRL_CLR    digitalWrite(RST_Pin,LOW)

#define TP_PRES_DOWN 0x80   
#define TP_CATH_PRES 0x40 

GT911::GT911(int16_t int_pin, int16_t rst_pin, int16_t w, int16_t h)
{
    INT_Pin = int_pin;
    RST_Pin = rst_pin;
    Width = w;
    Height = h;
}

void AddrScan()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    touchWire.beginTransmission(address);
    error = touchWire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(1000); // wait 5 seconds for next scan
}

bool GT911::init(uint8_t dir)
{
    bool retval = false;
    uint8_t touchIC_ID[4];
    
    Dir = dir % 4;
    pinMode(INT_Pin, INPUT);
    if(RST_Pin != -1)
    {
        pinMode(RST_Pin, OUTPUT);
        reset();
    }
    
    touchWire.begin();
    AddrScan();
    
    readData(GT9XX_ID_ADDR, 4, touchIC_ID);
    if(touchIC_ID[0] == '9')
    {
        retval = true;
    }
    
    return retval;
}

void GT911::reset()
{
    if(RST_Pin == -1)
        return;
    
    RST_CTRL_CLR;
    delay_ms(10);
    RST_CTRL_SET;
    delay_ms(10);
}

void GT911::writeHandle (uint16_t addr)
{
    touchWire.beginTransmission(GT9XX_IIC_WADDR); //д����ָ��
    touchWire.write((uint8_t)(addr >> 8));//д��16λ��ַ
    touchWire.write((uint8_t)addr);
//    touchWire.endTransmission();
}

void GT911::writeData (uint16_t addr, uint8_t value)
{
    writeHandle(addr);
    touchWire.write(value);
    touchWire.endTransmission();
}

void GT911::readData (uint16_t addr, uint8_t cnt, uint8_t *value)
{
    writeHandle(addr);
    
    delay_us(20);
    touchWire.write(GT9XX_IIC_RADDR);
    touchWire.write(addr);

    for(uint8_t i = 0 ; i < cnt; i++)
    {
        if (i == (cnt - 1))
        {
            value[i] = touchWire.requestFrom(addr, 1);
        }
        else
        {
            value[i] = touchWire.requestFrom(addr, 1);
        }
    }
    touchWire.endTransmission();
}

uint8_t GT911::getCount(void)
{
    uint8_t count[1] = {0};
    readData (GT9XX_READ_ADDR, 1, count); //read touch data
    return (count[0] & 0x0f);
}

uint8_t GT911::scan()
{
    uint8_t buf[42];
    uint8_t i = 0;
    uint8_t res = 0;
    uint8_t temp;
    uint8_t tempsta;

    static uint8_t t = 0; //���Ʋ�ѯ���,�Ӷ�����CPUռ����
    t++;
    if((t % 10) == 0 || t < 10) //����ʱ,ÿ����10��CTP_Scan�����ż��1��,�Ӷ���ʡCPUʹ����
    {
        readData(GT9XX_READ_ADDR, 42, buf);
        if((buf[0] & 0X80) && ((buf[0] & 0XF) < 6))
        {
            temp = 0;
            writeData(GT9XX_READ_ADDR, temp);
        }
        if((buf[0] & 0XF) && ((buf[0] & 0XF) < 6))
        {
            for(i = 0; i < (buf[0] & 0XF); i++)
            {
                switch(buf[1 + i * 8])
                {
                case 4:
                {
                    temp |= 1 << 4;
                    break;
                }
                case 3:
                {
                    temp |= 1 << 3;
                    break;
                }
                case 2:
                {
                    temp |= 1 << 2;
                    break;
                }
                case 1:
                {
                    temp |= 1 << 1;
                    break;
                }
                case 0:
                {
                    temp |= 1 << 0;
                    break;
                }
                default:
                    break;
                }
            }
//          temp=0XFF<<(buf[0]&0XF);        //����ĸ���ת��Ϊ1��λ��,ƥ��Status����
            tempsta = Status;       //���浱ǰ��Statusֵ
            Status = temp | TP_PRES_DOWN | TP_CATH_PRES;
            point[4].x = point[0].x;  //���津��0������
            point[4].y = point[0].y;
            for(i = 0; i < 5; i++)
            {
                if(Status & (1 << i)) //������Ч?
                {
                    //GT9XX_ReadData(TPX[i],4,buf); //��ȡXY����ֵ
                    if(Dir == 0)
                    {
                        point[i].x = ((uint16_t)buf[3 + i * 8] << 8) + buf[2 + i * 8];
                        point[i].y = ((uint16_t)buf[5 + i * 8] << 8) + buf[4 + i * 8];
                    }
                    else if(Dir == 1)
                    {
                        point[i].y = Height - (((uint16_t)buf[3 + i * 8] << 8) + buf[2 + i * 8]);
                        point[i].x = ((uint16_t)buf[5 + i * 8] << 8) + buf[4 + i * 8];
                    }
                    else if(Dir == 2)
                    {
                        point[i].x = Width - (((uint16_t)buf[3 + i * 8] << 8) + buf[2 + i * 8]);
                        point[i].y = Height - (((uint16_t)buf[5 + i * 8] << 8) + buf[4 + i * 8]);
                    }
                    else if(Dir == 3)
                    {
                        point[i].y = ((uint16_t)buf[3 + i * 8] << 8) + buf[2 + i * 8];
                        point[i].x = Width - (((uint16_t)buf[5 + i * 8] << 8) + buf[4 + i * 8]);
                    }
                    //printf("x[%d]:%d,y[%d]:%d\r\n",i,point[i].x,i,point[i].y);
                }
            }
            res = 1;
            if(point[0].x > Width || point[0].y > Height) //�Ƿ�����(���곬����)
            {
                if((buf[0] & 0XF) > 1)  //��������������,�򸴵ڶ�����������ݵ���һ������.
                {
                    point[0].x = point[1].x;
                    point[0].y = point[1].y;
                    t = 0;              //����һ��,��������������10��,�Ӷ����������
                }
                else                    //�Ƿ�����,����Դ˴�����(��ԭԭ����)
                {
                    point[0].x = point[4].x;
                    point[0].y = point[4].y;
                    buf[0] = 0X80;
                    Status = tempsta; //�ָ�Status
                }
            }
            else t = 0;                 //����һ��,��������������10��,�Ӷ����������
        }
    }
    if((buf[0] & 0X8F) == 0X80) //�޴����㰴��
    {
        if(Status & TP_PRES_DOWN) //֮ǰ�Ǳ����µ�
        {
            Status &= ~(1 << 7); //��ǰ����ɿ�
        }
        else                        //֮ǰ��û�б�����
        {
            point[0].x = 0xffff;
            point[0].y = 0xffff;
            Status &= 0XE0; //�������Ч���
        }
    }
    if(t > 240)t = 10; //���´�10��ʼ����
    return res;
}


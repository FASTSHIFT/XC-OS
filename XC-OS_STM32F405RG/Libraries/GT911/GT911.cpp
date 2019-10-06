#include "GT911.h"

typedef struct
{
    uint8_t dir;
    int width;
    int height;
} LCD_TypeDef;

LCD_TypeDef lcddev = {0, 320, 480};

/*****************************************************************************
 * @name       :void GT911_int_sync(uint16_t ms)
 * @date       :2019-06-20
 * @function   :GT911 initialization synchronization
 * @parameters :ms:Synchronization delay
 * @retvalue   :None
******************************************************************************/
void GT911_int_sync(uint16_t ms)
{
    INT_CTRL_CLR;
    delay_ms(ms);
    INT_IN();
}

/*****************************************************************************
 * @name       :void GT911_gpio_init(void)
 * @date       :2019-06-20
 * @function   :GT911 IC gpio initialization
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void GT911_gpio_init(void)
{
    GT_IIC_Init();
    RST_OUT();
    INT_OUT();
}

/*****************************************************************************
 * @name       :uint8_t GT9XX_WriteHandle (uint16_t addr)
 * @date       :2019-06-20
 * @function   :GT911 IC write handle
 * @parameters :addr:register address
 * @retvalue   :the status of writen
******************************************************************************/
uint8_t GT9XX_WriteHandle (uint16_t addr)
{
    uint8_t status;

    GT_IIC_Start();
    GT_IIC_Send_Byte(GT9XX_IIC_WADDR); //д����ָ��
    GT_IIC_Send_Byte((uint8_t)(addr >> 8)); //д��16λ��ַ
    GT_IIC_Send_Byte((uint8_t)addr);

    status = SUCCESS;
    return status;
}

/*****************************************************************************
 * @name       :uint8_t GT9XX_WriteData (uint16_t addr,uint8_t value)
 * @date       :2019-06-20
 * @function   :GT911 write data to register
 * @parameters :addr:register address
                value:the value for writen
 * @retvalue   :the status of writen
******************************************************************************/
uint8_t GT9XX_WriteData (uint16_t addr, uint8_t value)
{
    uint8_t status;
    GT_IIC_Start();

    GT9XX_WriteHandle(addr);
    GT_IIC_Send_Byte(value);
    GT_IIC_Stop();
    status = SUCCESS;
    return status;
}

/*****************************************************************************
 * @name       :uint8_t GT9XX_ReadData (uint16_t addr, uint8_t cnt, uint8_t *value)
 * @date       :2019-06-20
 * @function   :GT911 read data to register
 * @parameters :addr:register address
                cnt:the number of data which is read
                value:the point of read data
 * @retvalue   :the status of writen
******************************************************************************/
uint8_t GT9XX_ReadData (uint16_t addr, uint8_t cnt, uint8_t *value)
{
    uint8_t status;
    uint8_t i;

    status = ERROR;
    GT_IIC_Start();
    GT9XX_WriteHandle(addr);
    GT_IIC_Start();
    delay_us(20);
    GT_IIC_Send_Byte(GT9XX_IIC_RADDR);

    for(i = 0 ; i < cnt; i++)
    {
        if (i == (cnt - 1))
        {
            value[i] = GT_IIC_Read_Byte(0);
        }
        else
        {
            value[i] = GT_IIC_Read_Byte(1);
        }
    }
    GT_IIC_Stop();
    status = SUCCESS;
    return (status);
}

/*****************************************************************************
 * @name       :uint8_t GT911_Init(void)
 * @date       :2019-06-20
 * @function   :GT911 IC initialization
 * @parameters :None
 * @retvalue   :the status of initialization
******************************************************************************/
uint8_t GT911_Init(void)
{
    uint8_t touchIC_ID[4];
    GT911_gpio_init();
    GT911_int_sync(50);
    GT9XX_ReadData (GT9XX_ID_ADDR, 4, touchIC_ID);
    if( touchIC_ID[0] == '9' )
    {
        //printf("Touch ID: %s \r\n",touchIC_ID);
        return 1;
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************
 * @name       :uint8_t Touch_Get_Count(void)
 * @date       :2019-06-20
 * @function   :get the count of read data
 * @parameters :None
 * @retvalue   :the count of read data
******************************************************************************/
uint8_t Touch_Get_Count(void)
{
    uint8_t count[1] = {0};
    GT9XX_ReadData (GT9XX_READ_ADDR, 1, count); //read touch data
    return (count[0] & 0x0f);
}

/*****************************************************************************
 * @name       :uint8_t GT911_Scan(void)
 * @date       :2019-06-20
 * @function   :Query GT911 trigger status
 * @parameters :None
 * @retvalue   :the status of GT911 trigger
******************************************************************************/
uint8_t GT911_Scan(void)
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
        GT9XX_ReadData(GT9XX_READ_ADDR, sizeof(buf), buf);
        if((buf[0] & 0X80) && ((buf[0] & 0XF) < 6))
        {
            temp = 0;
            GT9XX_WriteData(GT9XX_READ_ADDR, temp);
        }
        if((buf[0] & 0XF) && ((buf[0] & 0XF) < 6))
        {
            for(i = 0; i < (buf[0] & 0XF); i++)
            {
                if(i <= 4)
                {
                    temp |= 1 << buf[1 + i * 8];
                }
            }

            tempsta = tp_dev.sta;       //���浱ǰ��tp_dev.staֵ
            tp_dev.sta = temp | TP_PRES_DOWN | TP_CATH_PRES;
            tp_dev.x[4] = tp_dev.x[0];  //���津��0������
            tp_dev.y[4] = tp_dev.y[0];
            for(i = 0; i < 5; i++)
            {
                if(tp_dev.sta & (1 << i)) //������Ч?
                {
                    //GT9XX_ReadData(TPX[i],4,buf); //��ȡXY����ֵ
                    if(lcddev.dir == 0)
                    {
                        tp_dev.x[i] = ((uint16_t)buf[3 + i * 8] << 8) + buf[2 + i * 8];
                        tp_dev.y[i] = ((uint16_t)buf[5 + i * 8] << 8) + buf[4 + i * 8];
                    }
                    else if(lcddev.dir == 1)
                    {
                        tp_dev.y[i] = lcddev.height - (((uint16_t)buf[3 + i * 8] << 8) + buf[2 + i * 8]);
                        tp_dev.x[i] = ((uint16_t)buf[5 + i * 8] << 8) + buf[4 + i * 8];
                    }
                    else if(lcddev.dir == 2)
                    {
                        tp_dev.x[i] = lcddev.width - (((uint16_t)buf[3 + i * 8] << 8) + buf[2 + i * 8]);
                        tp_dev.y[i] = lcddev.height - (((uint16_t)buf[5 + i * 8] << 8) + buf[4 + i * 8]);
                    }
                    else if(lcddev.dir == 3)
                    {
                        tp_dev.y[i] = ((uint16_t)buf[3 + i * 8] << 8) + buf[2 + i * 8];
                        tp_dev.x[i] = lcddev.width - (((uint16_t)buf[5 + i * 8] << 8) + buf[4 + i * 8]);
                    }
                    //printf("x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);
                }
            }
            res = 1;
            if(tp_dev.x[0] > lcddev.width || tp_dev.y[0] > lcddev.height) //�Ƿ�����(���곬����)
            {
                if((buf[0] & 0XF) > 1)  //��������������,�򸴵ڶ�����������ݵ���һ������.
                {
                    tp_dev.x[0] = tp_dev.x[1];
                    tp_dev.y[0] = tp_dev.y[1];
                    t = 0;              //����һ��,��������������10��,�Ӷ����������
                }
                else                    //�Ƿ�����,����Դ˴�����(��ԭԭ����)
                {
                    tp_dev.x[0] = tp_dev.x[4];
                    tp_dev.y[0] = tp_dev.y[4];
                    buf[0] = 0X80;
                    tp_dev.sta = tempsta; //�ָ�tp_dev.sta
                }
            }
            else t = 0;                 //����һ��,��������������10��,�Ӷ����������
        }
    }
    if((buf[0] & 0X8F) == 0X80) //�޴����㰴��
    {
        if(tp_dev.sta & TP_PRES_DOWN) //֮ǰ�Ǳ����µ�
        {
            tp_dev.sta &= ~(1 << 7); //��ǰ����ɿ�
        }
        else                        //֮ǰ��û�б�����
        {
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
            tp_dev.sta &= 0XE0; //�������Ч���
        }
    }
    if(t > 240)t = 10; //���´�10��ʼ����
    return res;
}


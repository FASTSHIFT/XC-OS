#include "touch.h"
#include "math.h"

Touch_TypeDef tp_dev =
{
    TP_Init,
    NULL,
    0,
    0,
    0,
    0,
};
//Ĭ��Ϊtouchtype=0������.

/*****************************************************************************
 * @name       :u8 TP_Init(void)
 * @date       :2018-08-09
 * @function   :Initialization touch screen
 * @parameters :None
 * @retvalue   :0-no calibration
                                1-Has been calibrated
******************************************************************************/
uint8_t TP_Init(void)
{
    GT911_Init();
    tp_dev.scan = GT911_Scan; //ɨ�躯��ָ��GT911������ɨ��
    return 0;
}



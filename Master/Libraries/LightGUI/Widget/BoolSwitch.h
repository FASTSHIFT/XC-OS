#ifndef __LG_BOOLSWITCH_H
#define __LG_BOOLSWITCH_H

#include "WidgetBasic.h"

namespace LightGUI {

/******  �������ؿؼ�  ******/
/******   BoolSwitch   ******/
template <class T> class BoolSwitch : public WidgetBasic<T> {
public:
    BoolSwitch(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r);

    uint16_t Color_SW;//������ɫ
    uint16_t Color_SW_BG;//����ʱ�Ŀ��ص�ɫ
    uint16_t Color_FM;//�����ɫ
    int16_t R;//��ť�뾶
    bool SwitchState, SwitchState_Last;
    virtual void display();
    void SetSwitch(bool sw);
private:
};

/**
  * @brief  �������ؿؼ����캯��
  * @param  obj: ������Ļ����
  * @param  x:   x����
  * @param  y:   y����
  * @param  w:   �ؼ����
  * @param  h:   �ؼ��߶�
  * @param  r:   ��ť�뾶
  * @retval ��
  */
template <class T> BoolSwitch<T>::BoolSwitch(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
    : WidgetBasic<T>(obj, x, y, w, h)
{
    Color_FM = Color_SW = WIDGET_Default_WG;
    Color_SW_BG = WIDGET_Default_BG;
    R = r;
}

/**
  * @brief  ��ʾ��������
  * @param  ��
  * @retval ��
  */
template <class T> void BoolSwitch<T>::display()
{
    this->screen->drawRoundRect(this->X, this->Y, this->Width, this->Height, R, Color_FM);
    if(SwitchState)
    {
        this->screen->fillRect(this->X + 1, this->Y + 1, this->Width - 2, this->Height - 2, Color_SW_BG);
        this->screen->fillCircle(this->X + this->Width - 3 - R, (this->Y + this->Y + this->Height) / 2, R, Color_SW);
    }
    else
    {
        this->screen->fillRect(this->X + 1, this->Y + 1, this->Width - 2, this->Height - 2, this->Color_BG);
        this->screen->fillCircle(this->X + 2 + R, (this->Y + this->Y + this->Height) / 2, R, Color_SW);
    }
    this->isDisplay = true;
}

/**
  * @brief  ���ò�������
  * @param  sw: true�򿪣�false�ر�
  * @retval ��
  */
template <class T> void BoolSwitch<T>::SetSwitch(bool sw)
{
    SwitchState = sw;
    if(SwitchState == SwitchState_Last && this->isDisplay) return;

    display();
    SwitchState_Last = SwitchState;
}

}

#endif

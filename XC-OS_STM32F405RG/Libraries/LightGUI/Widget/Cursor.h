#ifndef __LG_CURSOR_H
#define __LG_CURSOR_H

#include "WidgetBasic.h"

namespace LightGUI
{

/******  ���ؼ�  ******/
/******   Cursor   ******/
template <class T> class Cursor : public WidgetBasic<T>
{
    typedef void(*CursorCallbackFunction_t)(void);
public:
    Cursor(T *obj, int16_t x, int16_t y, int16_t w, int16_t h);

    uint16_t Color_CS;//�����ɫ
    uint8_t Style;//�������
    CursorCallbackFunction_t DisplayCallbackFunction;//����ػ�ص�����

    virtual void display();
private:
};

/**
  * @brief  ָ��ؼ����캯��
  * @param  obj: ������Ļ����
  * @param  x:   x����
  * @param  y:   y����
  * @param  w:   �ؼ����
  * @param  h:   �ؼ��߶�
  * @retval ��
  */
template <class T> Cursor<T>::Cursor(T *obj, int16_t x, int16_t y, int16_t w, int16_t h)
    : WidgetBasic<T>(obj, x, y, w, h)
{
    Color_CS = WIDGET_Default_WG;
    Style = 0;
    DisplayCallbackFunction = 0;
}

/**
  * @brief  ��ʾ���
  * @param  ��
  * @retval ��
  */
template <class T> void Cursor<T>::display()
{
    if(DisplayCallbackFunction)
        DisplayCallbackFunction();
    else
    {
        switch(Style)
        {
        case 0:
            this->screen->fillRect(this->X, this->Y, this->Width, this->Height, Color_CS);
            break;
        case 1:
            this->screen->drawRect(this->X_Last, this->Y_Last, this->Width_Last, this->Height_Last, this->Color_BG);
            this->screen->drawRect(this->X, this->Y, this->Width, this->Height, Color_CS);
            break;
        }
    }
    this->isDisplay = true;
}

}

#endif

#ifndef __LG_JOYSTICK_H
#define __LG_JOYSTICK_H

#include "WidgetBasic.h"

namespace LightGUI {

#ifndef constrain
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

/******   ҡ�˿ؼ�   ******/
/******  Joystick ******/
template <class T> class Joystick : public WidgetBasic<T> {
public:
    Joystick(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t r);

    uint16_t Color_JS, Color_FM;//ҡ����ɫ, �����ɫ
    int16_t R;                  //ҡ����뾶
    int16_t X_Js, Y_Js;         //ҡ��������
    float Xc, Yc;               //ҡ����ӳ������(-1.0 ~ 1.0)

    virtual void display();
    void setJsPos(float x, float y);//����ҡ��ӳ������(-1.0 ~ 1.0)
private:
    int16_t X_JsLast, Y_JsLast;  //��һ�ε�ҡ������
    bool UpdateAll;
    void UpdateJs();
};

/**
  * @brief  ҡ�˿ؼ����캯��
  * @param  obj: ������Ļ����
  * @param  x:   x����
  * @param  y:   y����
  * @param  w:   �ؼ����
  * @param  h:   �ؼ��߶�
  * @param  r:   ҡ����뾶
  * @retval ��
  */
template <class T> Joystick<T>::Joystick(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t r)
    : WidgetBasic<T>(obj, x, y, w, h)
{
    Color_FM = Color_JS = WIDGET_Default_WG;
    R = r;

    X_JsLast = X_Js = (2 * this->X + this->Width) / 2;
    Y_JsLast = Y_Js = (2 * this->Y + this->Height) / 2;
    UpdateAll = false;
}

/**
  * @brief  ����ҡ��������
  * @param  ��
  * @retval ��
  */
template <class T> void Joystick<T>::UpdateJs()
{
    X_Js = fmap(Xc, -1.0, 1.0, this->X + R + 1, this->X + this->Width - R - 2);
    Y_Js = fmap(Yc, -1.0, 1.0, this->Y + R + 1, this->Y + this->Height - R - 2);
}

/**
  * @brief  ��ʾҡ��
  * @param  ��
  * @retval ��
  */
template <class T> void Joystick<T>::display()
{ 	
    if(UpdateAll)
    {
        this->screen->drawRect(this->X, this->Y, this->Width, this->Height, Color_FM);
        this->screen->fillRect(this->X + 1, this->Y + 1, this->Width - 2, this->Height - 2, this->Color_BG); 
    }
    else
	{
		if(this->isDisplay)
			this->screen->fillCircle(X_JsLast, Y_JsLast, R, this->Color_BG);
	}
    
	UpdateJs();
    this->screen->fillCircle(X_Js, Y_Js, R, Color_JS);
    X_JsLast = X_Js, Y_JsLast = Y_Js;
    
    this->isDisplay = true;
    UpdateAll = true;
}

/**
  * @brief  ����ҡ������
  * @param  x:   x����(-1.0~1.0)
  * @param  y:   y����(-1.0~1.0)
  * @retval ��
  */
template <class T> void Joystick<T>::setJsPos(float x, float y)
{
    x = constrain(x, -1.0f, 1.0f);
    y = constrain(y, -1.0f, 1.0f);
    Xc = x, Yc = -y;
    UpdateJs();
    if(X_Js == X_JsLast && Y_Js == Y_JsLast) return;
    
    UpdateAll = false;
    display();
}

}

#endif

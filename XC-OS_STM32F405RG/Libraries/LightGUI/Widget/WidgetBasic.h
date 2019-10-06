#ifndef __LG_WIDGETBASIC_H
#define __LG_WIDGETBASIC_H

#include "stdint.h"

namespace LightGUI{

#define WIDGET_Default_BG T::Black//Ĭ�Ͽؼ���ɫ
#define WIDGET_Default_WG T::White//Ĭ�Ͽؼ���ɫ

#ifndef ABS
#define ABS(x) (((x)>0)?(x):-(x))
#endif

/****** �ؼ������� ******/
template <class T> class WidgetBasic { 
public:
    WidgetBasic(T *obj, int16_t x, int16_t y, int16_t w, int16_t h);
    ~WidgetBasic();

    T *screen;//��Ļ����ָ��
    bool isDisplay;        //��ʾ��־λ
    int16_t X, Y;          //�ؼ�����
    int16_t Width, Height; //�ؼ����
    int16_t X_Last, Y_Last;         //��һ�οؼ�����
    int16_t Width_Last, Height_Last;//��һ�οؼ����
    uint16_t Color_BG;     //�ؼ���ɫ

    virtual void display(); //��ʾ�ؼ�
    void noDisplay();       //����ʾ�ؼ�
    void setPosition(int16_t x, int16_t y); //���ÿؼ�����
    void setPosition_f(float x, float y);   //���ÿؼ��������Ļ��С
    void setSize(int16_t w, int16_t h);     //���ÿؼ���С
    void updatePosition();  //���¿ؼ�λ��
private:   
};

/**
  * @brief  �ؼ������๹�캯��
  * @param  obj: ������Ļ����
  * @param  x:   x����
  * @param  y:   y����
  * @param  w:   �ؼ����
  * @param  h:   �ؼ��߶�
  * @retval ��
  */
template <class T> WidgetBasic<T>::WidgetBasic(T *obj, int16_t x, int16_t y, int16_t w, int16_t h)
{
    screen = obj;
    X = X_Last = x;//����Ĭ�ϲ���
    Y = Y_Last = y;
    Width = Width_Last = w;
    Height = Height_Last = h;
    Color_BG = WIDGET_Default_BG;
    isDisplay = false;
}

/**
  * @brief  �ؼ���������������
  * @param  ��
  * @retval ��
  */
template <class T> WidgetBasic<T>::~WidgetBasic()
{
    noDisplay();
}

/**
  * @brief  ��ʾ�ؼ�
  * @param  ��
  * @retval ��
  */
template <class T> void WidgetBasic<T>::display()
{
}

/**
  * @brief  ����ʾ�ؼ�
  * @param  ��
  * @retval ��
  */
template <class T> void WidgetBasic<T>::noDisplay()
{
    screen->fillRect(X, Y, Width, Height, Color_BG);
    isDisplay = false;
}

/**
  * @brief  ���¿ؼ�λ��
  * @param  ��
  * @param  ��
  * @retval ��
  */
template <class T> void WidgetBasic<T>::updatePosition()
{
    if(X == X_Last && Y == Y_Last && isDisplay)return;

    if(!isDisplay)
        X_Last = X, Y_Last = Y;

    if((ABS(X - X_Last) > Width) || (ABS(Y - Y_Last) > Height))
    {
        screen->fillRect(X_Last, Y_Last, Width, Height, Color_BG);
    }
    else
    {
        if(X > X_Last)
        {
            screen->fillRect(X_Last, Y_Last, X - X_Last, Height, Color_BG);
        }
        else if(X < X_Last)
        {
            screen->fillRect(X + Width, Y_Last, X_Last - X, Height, Color_BG);
        }

        if(Y > Y_Last)
        {
            screen->fillRect(X_Last, Y_Last, Width, Y - Y_Last, Color_BG);
        }
        else if(Y < Y_Last)
        {
            screen->fillRect(X_Last, Y + Height, Width, Y_Last - Y, Color_BG);
        }
    }
    display();
    X_Last = X, Y_Last = Y; 
}

/**
  * @brief  ���ÿؼ�����
  * @param  x:   x����
  * @param  y:   y����
  * @retval ��
  */
template <class T> void WidgetBasic<T>::setPosition(int16_t x, int16_t y)
{
    X = x;
    Y = y;
    updatePosition();
}

template <class T> void WidgetBasic<T>::setPosition_f(float x, float y)
{
    setPosition(screen->width() * x - Width / 2, screen->height() * y - Height / 2);
}

/**
  * @brief  ���ÿؼ����
  * @param  w:   �ؼ����
  * @param  h:   �ؼ��߶�
  * @retval ��
  */
template <class T> void WidgetBasic<T>::setSize(int16_t w, int16_t h)
{
    Width = w;
    Height = h;
    if(Width == Width_Last && Height == Height_Last && isDisplay)//�ж��¿���Ƿ���ɿ���ظ�
        return;

    if(!isDisplay)
        Width_Last = Width, Height_Last = Height;

    screen->fillRect(X, Y, Width_Last, Height_Last, Color_BG);
    display();
    Width_Last = Width, Height_Last = Height;
}

}

#endif

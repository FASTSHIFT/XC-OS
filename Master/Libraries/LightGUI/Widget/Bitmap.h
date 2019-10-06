#ifndef __LG_BITMAP_H
#define __LG_BITMAP_H

#include "WidgetBasic.h"

namespace LightGUI {

/******  ��ͼ�ؼ�   ******/
/******   Bitmap    ******/
template <class T> class Bitmap {
    typedef void(*FastDrawRGBBitmapFunction_t)(int16_t, int16_t, uint16_t*, int16_t, int16_t);
    typedef void(*FastDrawBitmapFunction_t)(int16_t, int16_t, uint8_t*, int16_t, int16_t, uint8_t, uint8_t);
public:
    Bitmap(T *obj, uint16_t* bitmap_p, int16_t x, int16_t y, int16_t w, int16_t h);
    Bitmap(T *obj, uint8_t* bitmap_p, int16_t x, int16_t y, int16_t w, int16_t h);
    uint16_t Color_BG;          //��ͼ����ɫ
    int16_t X, Y, Width, Height;//��ͼ����,����

    FastDrawRGBBitmapFunction_t FastDrawRGBBitmapCallback;
    FastDrawBitmapFunction_t    FastDrawBitmapCallback;
    void display();             //��ʾ��ͼ
    void noDisplay();           //����ʾ��ͼ
    void setPosition(int16_t x, int16_t y);//������ͼ����
    void setPosition_f(float x, float y);//������ͼ��Ļӳ������(0.0 ~ 1.0)
    void updatePosition();      //������ͼλ��
private:
    T *screen;  //��Ļ����ָ��
    uint16_t* rgbbitmap;           //ָ��RGB��ͼ��ָ��
    uint8_t* bitmap;           //ָ��RGB��ͼ��ָ��
    bool isDisplay;             //��ʾ��־λ
    int16_t X_Last, Y_Last;     //��һ�οؼ�����
};

/**
  * @brief  RGB��ͼ�ؼ����캯��
  * @param  obj: ������Ļ����
  * @param  bitmap_p: ָ��RGB��ͼ��ָ��
  * @param  x:   x����
  * @param  y:   y����
  * @param  w:   �ؼ����
  * @param  h:   �ؼ��߶�
  * @retval ��
  */
template <class T> Bitmap<T>::Bitmap(T *obj, uint16_t* bitmap_p, int16_t x, int16_t y, int16_t w, int16_t h)
{
    screen = obj;
    rgbbitmap = bitmap_p;
    bitmap = 0;
    X = X_Last = x;
    Y = Y_Last = y;
    Width = w;
    Height = h;
    Color_BG = WIDGET_Default_BG;
    FastDrawRGBBitmapCallback = 0;
    FastDrawBitmapCallback = 0;
    isDisplay = false;
}

/**
  * @brief  ��ɫ��ͼ�ؼ����캯��
  * @param  obj: ������Ļ����
  * @param  bitmap_p: ָ����ͼ��ָ��
  * @param  x:   x����
  * @param  y:   y����
  * @param  w:   �ؼ����
  * @param  h:   �ؼ��߶�
  * @retval ��
  */
template <class T> Bitmap<T>::Bitmap(T *obj, uint8_t* bitmap_p, int16_t x, int16_t y, int16_t w, int16_t h)
{
    screen = obj;
    rgbbitmap = 0;
    bitmap = bitmap_p;
    X = X_Last = x;
    Y = Y_Last = y;
    Width = w;
    Height = h;
    Color_BG = WIDGET_Default_BG;
    FastDrawRGBBitmapCallback = 0;
    FastDrawBitmapCallback = 0;
    isDisplay = false;
}

/**
  * @brief  ��ʾ��ͼ
  * @param  ��
  * @retval ��
  */
template <class T> void Bitmap<T>::display()
{
    if(bitmap)
    {
        if(FastDrawBitmapCallback)
            FastDrawBitmapCallback(X, Y, bitmap, Width, Height, !Color_BG, Color_BG);
        else
            screen->drawBitmap(X, Y, bitmap, Width, Height, !Color_BG, Color_BG);
    }
    else if(rgbbitmap)
    {
        if(FastDrawRGBBitmapCallback)
            FastDrawRGBBitmapCallback(X, Y, rgbbitmap, Width, Height);
        else
            screen->drawRGBBitmap(X, Y, rgbbitmap, Width, Height);
    }
    isDisplay = true;
}

/**
  * @brief  ����ʾ��ͼ
  * @param  ��
  * @retval ��
  */
template <class T> void Bitmap<T>::noDisplay()
{
    screen->fillRect(X, Y, Width, Height, Color_BG);
    isDisplay = false;
}

/**
  * @brief  ������ͼλ��
  * @param  ��
  * @param  ��
  * @retval ��
  */
template <class T> void Bitmap<T>::updatePosition()
{
    if(X == X_Last && Y == Y_Last && isDisplay)return;

    if(!isDisplay)X_Last = X, Y_Last = Y;

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
    X_Last = X, Y_Last = Y;
    display();
}

/**
  * @brief  ������ͼ����
  * @param  x:   x����
  * @param  y:   y����
  * @retval ��
  */
template <class T> void Bitmap<T>::setPosition(int16_t x, int16_t y)
{
    X = x;
    Y = y;
    updatePosition();
}

/**
  * @brief  ������ͼ��Ļӳ������
  * @param  x:   x����(0.0 ~ 1.0)
  * @param  y:   y����(0.0 ~ 1.0)
  * @retval ��
  */
template <class T> void Bitmap<T>::setPosition_f(float x, float y)
{
    setPosition(screen->width() * x - Width / 2, screen->height() * y - Height / 2);
}

}

#endif

#ifndef __LG_SCROLLBAR_H
#define __LG_SCROLLBAR_H

#include "WidgetBasic.h"

namespace LightGUI{

/******  �������ؼ�  ******/
/******  ScrollBar   ******/
template <class T> class ScrollBar : public WidgetBasic<T> {
public:
    ScrollBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint16_t l);

    uint16_t Color_SB, Color_FM;//��������ɫ, �����ɫ
    int16_t Scroll, Long;//��������, ����������
    uint8_t Dir;//����(0:����, 1:����)

    virtual void display();
    void setScroll(float scroll);//���ù���������(0.0~1.0 -> 0~100%)
    inline void setProgress(float scroll) {
        setScroll(scroll);
    };
private:
    int16_t Scroll_Last;
};

/**
  * @brief  �������ؼ����캯��
  * @param  obj: ������Ļ����
  * @param  x:   x����
  * @param  y:   y����
  * @param  w:   �ؼ����
  * @param  h:   �ؼ��߶�
  * @param  dir: ����(0:����, 1:����)
  * @param  l:   ����������
  * @retval ��
  */
template <class T> ScrollBar<T>::ScrollBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint16_t l)
    : WidgetBasic<T>(obj, x, y, w, h)
{
    Color_SB = WIDGET_Default_WG;
    Color_FM = WIDGET_Default_WG;
    Long = l;
    Dir = dir;
}

/**
  * @brief  ��ʾ������
  * @param  ��
  * @retval ��
  */
template <class T> void ScrollBar<T>::display()
{
    this->screen->drawRect(this->X, this->Y, this->Width, this->Height, Color_FM);

    if(Dir == 0)
    {
        int16_t w1 = (this->Width - Long - 2) * (Scroll / 1000.0) + 1;
        int16_t x1 = this->X + w1;
        int16_t x2 = x1 + Long;
        int16_t w2 = this->Width - w1 - Long - 1;
        if(w1 > 0)this->screen->fillRect(this->X + 1, this->Y + 1, w1, this->Height - 2, this->Color_BG);
        this->screen->fillRect(x1, this->Y + 1, Long, this->Height - 2, Color_SB);
        if(w2 > 0)this->screen->fillRect(x2, this->Y + 1, w2, this->Height - 2, this->Color_BG);
    }
    else
    {
        int16_t h1 = (this->Height - Long - 2) * (1.0 - Scroll / 1000.0) + 1;
        int16_t y1 = this->Y + h1;
        int16_t y2 = y1 + Long;
        int16_t h2 = this->Height - h1 - Long - 1;
        if(h1 > 0)this->screen->fillRect(this->X + 1, this->Y + 1, this->Width - 2, h1, this->Color_BG);
        this->screen->fillRect(this->X + 1, y1, this->Width - 2, Long, Color_SB);
        if(h2 > 0)this->screen->fillRect(this->X + 1, y2, this->Width - 2, h2, this->Color_BG);

    }
    this->isDisplay = true;
}

/**
  * @brief  ���ù���������
  * @param  scroll: ����(0.0~1.0 -> 0~100%)
  * @retval ��
  */
template <class T> void ScrollBar<T>::setScroll(float scroll)
{
    scroll = constrain(scroll, 0.0f, 1.0f);
    Scroll = scroll * 1000.0f;
    if(Scroll == Scroll_Last && this->isDisplay) return;
    
    display();
    Scroll_Last = Scroll;
}

}

#endif

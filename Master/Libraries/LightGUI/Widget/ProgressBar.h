#ifndef __LG_PROGRESSBAR_H
#define __LG_PROGRESSBAR_H

#include "WidgetBasic.h"

namespace LightGUI {

/****** �������ؼ�  ******/
/****** ProgressBar ******/
template <class T> class ProgressBar : public WidgetBasic<T> {
public:
    ProgressBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint8_t style = 0);
    ProgressBar(T *obj, uint8_t dir, uint8_t style = 0);

    uint16_t Dir, Progress, Style; ////����(0:����, 1:����), ����, ����
    uint16_t Color_PB, Color_FM;   //��������ɫ, �����ɫ

    virtual void display();
    void setProgress(float progress);//���ý���(0.0~1.0 -> 0~100%)
private:
    uint16_t Progress_Last;//��һ�ν���
};

/**
  * @brief  ���������캯��
  * @param  obj: ������Ļ����
  * @param  dir: ����(0:����, 1:����)
  * @param  style:  ����
  * @retval ��
  */
template <class T> ProgressBar<T>::ProgressBar(T *obj, uint8_t dir, uint8_t style)
    : WidgetBasic<T>(obj, 0, 0, 0, 0)
{
    Dir = dir;
    Style = style;
    Color_PB = WIDGET_Default_WG;
    Color_FM = WIDGET_Default_WG;
}

/**
  * @brief  ���������캯��
  * @param  obj: ������Ļ����
  * @param  x:   x����
  * @param  y:   y����
  * @param  w:   �ؼ����
  * @param  h:   �ؼ��߶�
  * @param  dir: ����(0:����, 1:����)
  * @param  style:  ����
  * @retval ��
  */
template <class T> ProgressBar<T>::ProgressBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint8_t style)
    : WidgetBasic<T>(obj, x, y, w, h)
{
    Dir = dir;
    Style = style;
    Color_PB = WIDGET_Default_WG;
    Color_FM = WIDGET_Default_WG;
}

/**
  * @brief  ��ʾ������
  * @param  ��
  * @retval ��
  */
template <class T> void ProgressBar<T>::display()
{
    this->screen->drawRect(this->X, this->Y, this->Width, this->Height, Color_FM);
    if(Dir == 0)
    {
        int16_t w2 = (this->Width - 4) * (Progress / 1000.0);
        int16_t w3 = this->Width - 4 - w2;
        int16_t x3 = this->X + 2 + w2;

        if(w2 > 0)this->screen->fillRect(this->X + 2, this->Y + 2, w2, this->Height - 4, Color_PB);
        if(w3 > 0)this->screen->fillRect(x3, this->Y + 2, w3, this->Height - 4, this->Color_BG);
    }
    else
    {
        int16_t h2 = (this->Height - 4) * (Progress / 1000.0);
        int16_t h3 = this->Height - 4 - h2;
        int16_t y2 = this->Y + 2 + h3;

        if(h2 > 0)this->screen->fillRect(this->X + 2, y2, this->Width - 4, h2, this->Color_PB);
        if(h3 > 0)this->screen->fillRect(this->X + 2, this->Y + 2, this->Width - 4, h3, this->Color_BG);
    }
    this->isDisplay = true;
}

/**
  * @brief  ���ý���������
  * @param progress: ����(0.0~1.0 -> 0~100%)
  * @retval ��
  */
template <class T> void ProgressBar<T>::setProgress(float progress)
{
    progress = constrain(progress, 0.0f, 1.0f);
    Progress = progress * 1000.0f;
    if(Progress == Progress_Last && this->isDisplay)return;
    
    display();
    Progress_Last = Progress;
}

}

#endif

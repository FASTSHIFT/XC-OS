#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "BV_Player.h"

static uint8_t BvBuffer[128 * 64 * 2];

BV_Player BvPlayer(BvBuffer, sizeof(BvBuffer));
static lv_task_t * taskBvPlayer;

static String BvFilePath;
void Bv_SetFilePath(String path)
{
    BvFilePath = path;
}

static void Screen_FastDrawRGBBitmap(uint16_t *bitmap, BvHeader_TypeDef* head, int16_t *x, int16_t *y)
{
    screen.drawFastRGBBitmap(*x, *y, bitmap, head->Width, head->Height);
}

static void Screen_FastDrawBitmap(uint8_t *bitmap, BvHeader_TypeDef* head, int16_t *x, int16_t *y)
{
    screen.setAddrWindow(*x, *y, head->Width - 1 + *x, head->Height - 1 + *y);

    int16_t byteWidth = (head->Width + 7) / 8;
    uint8_t b = 0;

    for(int16_t j = 0; j < head->Height; j++)
    {
        for(int16_t i = 0; i < head->Width; i++ )
        {
            if(i & 7) b <<= 1;
            else      b   = bitmap[j * byteWidth + i / 8];
            screen.pushColor((b & 0x80) ? screen.White : screen.Black);
        }
    }
}

static void Task_BvPlyaerUpdate(lv_task_t * task)
{
    BvPlayer.UpdatePlayerState();
    if(BvPlayer.IsEnd())
    {
        page.PagePop();
    }
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    taskBvPlayer = lv_task_create(Task_BvPlyaerUpdate, 33, LV_TASK_PRIO_MID, 0);
    memset(BvBuffer, 0, sizeof(BvBuffer));
    if(!BvPlayer.OpenVideo(BvFilePath))
    {
        return;
    }
    BvPlayer.BaseX = (APP_WIN_WIDTH - BvPlayer.Head.Width) * 0.5f;
    BvPlayer.BaseY = (APP_WIN_HEIGHT - BvPlayer.Head.Height) * 0.5f;
    BvPlayer.SetPlayerState(BvPlayer.Run);
    BvPlayer.DrawFrame_16bit = Screen_FastDrawRGBBitmap;
    BvPlayer.DrawFrame_8bit = Screen_FastDrawBitmap;
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    BvPlayer.CloseVideo();
    lv_task_del(taskBvPlayer);
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        if(btn == btnBack)
        {
            page.PagePop();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_BvPlayer(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}

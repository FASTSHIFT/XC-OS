#include "BV_Player.h"

//#define DEBUG_SERIAL Serial

#ifdef DEBUG_SERIAL
#include "HardwareSerial.h"
#define DEBUG(data) DEBUG_SERIAL.print(data)
#else
#define DEBUG(data)
#endif

BV_Player::BV_Player(uint8_t* buffer, uint32_t bufferSizeMax, int16_t baseX, int16_t baseY)
{
    Buffer = buffer;
    BufferSizeMax = bufferSizeMax;
    BaseX = baseX;
    BaseY = baseY;

    PlayerStateSet = PlayerState = Close;
    NowFrame = StartFrame = 0;
}

bool BV_Player::OpenVideo(String path)
{
    BvFile = SD.open(path);
    if(BvFile)
    {
        if(BvFile.available())
        {
            BvFile.read(&Head, sizeof(BvHeader_TypeDef));

            FrameSize = Head.Width * Head.Height;

            if(Head.Type == BvType_RGB565)
                FrameSize *= sizeof(uint16_t);
            else if(Head.Type == BvType_BIN)
                FrameSize /= 8;

//            if(BufferSize > BufferSizeMax)
//            {
//                DEBUG("\r\nMemory too few");
//                BvFile.close();
//                return false;
//            }

            EndFrame = Head.FrameNum;
            NowFrame = 0;

            PlayerState = Ready;

            DEBUG("\r\n");
            DEBUG(path);
            DEBUG(" success opening");

            DEBUG("\r\nType:");
            DEBUG(Head.Type);

            DEBUG(" @");
            DEBUG(Head.Width);
            DEBUG('x');
            DEBUG(Head.Height);

            DEBUG("\r\nFrameNum:");
            DEBUG(Head.FrameNum);

            return true;
        }
        else
        {
            BvFile.close();
        }
    }
    return false;
}

void BV_Player::CloseVideo()
{
    if(BvFile)
    {
        BvFile.close();
        PlayerState = Close;
    }
}

void BV_Player::SetPlayerState(State_t state)
{
    if(!BvFile)
        return;

    if(state != PlayerState)
    {
        if(state == Close)
        {
            CloseVideo();
        }
        else if(state == Run || state == Pause || state == Stop)
        {
            PlayerState = state;
        }
        else if(state == Always)
        {
            PlayerStateSet = state;
            PlayerState = Run;
        }
    }
}

#define DRAW_PIC(b,x,y,w,h)\
do{\
    if(DrawPic_##b##bit)\
        DrawPic_##b##bit(x,y,(uint##b##_t*)Buffer,w,h);\
}while(0)


void BV_Player::DrawOneFrame()
{
    /*�������㹻��һ֡*/
    if(FrameSize <= BufferSizeMax)
    {
        BvFile.read(Buffer, FrameSize);
        if(Head.Type == BvType_RGB565)
            DRAW_PIC(16, BaseX, BaseY, Head.Width, Head.Height);
        else if(Head.Type == BvType_BIN)
            DRAW_PIC(8, BaseX, BaseY, Head.Width, Head.Height);
    }
    else
    {
        /*���㻺������ʹ�����*/
        volatile uint32_t buf_aera = BufferSizeMax;
        if(Head.Type == BvType_RGB565)
        {
            buf_aera = BufferSizeMax / sizeof(uint16_t);
        }
        else if(Head.Type == BvType_BIN)
        {
            buf_aera = BufferSizeMax * 8;
        }

        /*���������*/
        int16_t buf_w = Head.Width;
        /*�������߶�*/
        int16_t buf_h = buf_aera / buf_w;
        
        /*�Ƿ��ܻ���һ��*/
        if(buf_h == 0)
            return;
        
        /*֡�ܸ߶�*/
        int16_t frame_h = Head.Height;
        /*֡�߶��Ƿ��������������߶�*/
        bool isExactDiv = (frame_h % buf_h == 0);
        /*������Ⱦ����*/
        int16_t times = isExactDiv ? frame_h / buf_h : frame_h / buf_h + 1;
        /*y��ƫ����*/
        int16_t y_offset = 0;

        /*��ʼ��Ⱦ*/
        for(uint32_t i = 0; i < times; i++)
        {
            /*�Ƿ�������&&�Ƿ�Ϊ���һ����Ⱦ*/
            if(!isExactDiv && i == (times - 1))
            {
                /*���ʣ���ͼ��߶�*/
                buf_h = frame_h % buf_h;
            }

            if(Head.Type == BvType_RGB565)
            {
                BvFile.read(Buffer, buf_h * buf_w * sizeof(uint16_t));
                DRAW_PIC(16, BaseX, BaseY + y_offset, buf_w, buf_h);
            }
            else if(Head.Type == BvType_BIN)
            {
                BvFile.read(Buffer, buf_h * buf_w / 8);
                DRAW_PIC(8, BaseX, BaseY + y_offset, buf_w, buf_h);
            }
            /*y��ƫ�ƣ�׼����һ����Ⱦ*/
            y_offset += buf_h;
        }
    }
}

void BV_Player::UpdatePlayerState()
{
    if(PlayerState == Run)
    {
        if(NowFrame < EndFrame)
        {
            uint32_t start = millis();
            DrawOneFrame();
            FrameTime = millis() - start;
            NowFrame++;
        }
        else
        {
//          BvFile.seek(sizeof(Head));
//          NowFrame = 0;
            PlayerState = Stop;
        }
    }


    if(PlayerStateSet == Always)
    {
        if(PlayerState == Stop)
        {
            BvFile.seek(sizeof(Head));
            NowFrame = 0;
            PlayerState = Run;
        }
    }
    else if(PlayerStateSet == Stop)
    {
        BvFile.seek(sizeof(Head));
        NowFrame = 0;
        PlayerState = Ready;
    }
}

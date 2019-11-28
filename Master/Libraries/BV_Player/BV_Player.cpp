#include "BV_Player.h"

//#define DEBUG_SERIAL Serial

#ifdef DEBUG_SERIAL
#include "HardwareSerial.h"
#define DEBUG(data) DEBUG_SERIAL.print(data)
#else
#define DEBUG(data)
#endif

#define DRAW_FRAME(b)\
do{\
    if(DrawFrame_##b##bit)\
        DrawFrame_##b##bit((uint##b##_t*)Buffer, &Head, &BaseX, &BaseY);\
}while(0)

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
            
            BufferSize = Head.Width * Head.Height;
            
            if(Head.Type != BvType_BIN)
               BufferSize *= 2; 
            
            if(BufferSize > BufferSizeMax)
            {
                DEBUG("\r\nMemory too few");
                BvFile.close();
                return false;
            }

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

void BV_Player::UpdatePlayerState()
{
    if(PlayerState == Run)
    {
        if(NowFrame < EndFrame)
        {
            BvFile.read(Buffer, BufferSize);
            if(Head.Type == BvType_RGB565)
            {
                DRAW_FRAME(16);
            }
            else if(Head.Type == BvType_BIN)
            {
                DRAW_FRAME(8);
            }
            
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

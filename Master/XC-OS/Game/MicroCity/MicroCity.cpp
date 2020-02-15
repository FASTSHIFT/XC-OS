//#include <Arduboy2.h>
//#include <EEPROM.h>
#include "Draw.h"
#include "Interface.h"
#include "Game.h"
#include "Simulation.h"
#include "APP/Game/GamePrivate.h"
#include "Basic/TasksManage.h"

//Arduboy2Base arduboy;

uint8_t GetInput()
{
    uint8_t result = 0;

    if(Game_GetButtonPressed(A_BUTTON))
    {
        result |= INPUT_A;
    }
    if(Game_GetButtonPressed(B_BUTTON))
    {
        result |= INPUT_B;
    }
    if(Game_GetButtonPressed(UP_BUTTON))
    {
        result |= INPUT_UP;
    }
    if(Game_GetButtonPressed(DOWN_BUTTON))
    {
        result |= INPUT_DOWN;
    }
    if(Game_GetButtonPressed(LEFT_BUTTON))
    {
        result |= INPUT_LEFT;
    }
    if(Game_GetButtonPressed(RIGHT_BUTTON))
    {
        result |= INPUT_RIGHT;
    }

    return result;
}

void PutPixel(uint8_t x, uint8_t y, uint8_t color)
{
    Game_DispDrawPixel(x, y, color);
}

/*
void DrawFilledRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t colour)
{
  arduboy.fillRect(x, y, w, h, colour);
}

void DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t colour)
{
  arduboy.drawRect(x, y, w, h, colour);
}
*/

void DrawBitmap(const uint8_t* bmp, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    Game_DispDrawBitmap(x, y, bmp, w, h, 1);
}

void SaveCity()
{
    Game_WriteDataToFile(&State, sizeof(State), "MicroCity");
}

bool LoadCity()
{
    return Game_ReadDataFromFile(&State, sizeof(State), "MicroCity");
}

uint8_t* GetPowerGrid()
{
    return Game_DispGetBuffer();
}

static void Setup()
{
    InitGame();
}

static void CityLoop()
{
    if(!Game_DispBusy())
    {
        TickGame();
        Game_ClearButtonState();
        Game_DispReqUpdate();
    }
}

static void Loop()
{
    __IntervalExecute(CityLoop(), 1000 / GAME_FRAME_RATE);
}

void GameRegister_MicroCity(uint8_t gameID)
{
    GamePage.PageRegister(gameID, Setup, Loop, NULL, NULL);
}

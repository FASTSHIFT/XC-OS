#ifndef __GAMEPRIVATE_H
#define __GAMEPRIVATE_H

#include <stdint.h>
#include "Basic/CommonMacro.h"
#include "GUI/DisplayPrivate.h"

#define GAME_SCREEN_WIDTH  128
#define GAME_SCREEN_HEIGHT 64
#define GAME_FRAME_RATE    25           

extern PageManager GamePage;

enum {
    A_BUTTON,
    B_BUTTON,
    UP_BUTTON,
    DOWN_BUTTON,
    LEFT_BUTTON,
    RIGHT_BUTTON,
    MAX_BUTTON
};

enum {
    GAME_NONE,
    GAME_MicroCity,
    GAME_MAX
};

void Game_Init();

bool Game_GetButtonPressed(uint8_t btn_id);
void Game_SetButtonState(uint8_t btn_id, bool state);
void Game_ClearButtonState();


bool Game_WriteDataToFile(void* data, uint32_t len, const char* name);
bool Game_ReadDataFromFile(void* data, uint32_t len, const char* name);

void Game_DispGetContPos(int16_t* x, int16_t* y);
    
bool Game_DispBusy();
bool Game_DispReqUpdate();
void Game_DispTaskUpdate();
uint8_t* Game_DispGetBuffer();
void Game_DispDrawPixel(int16_t x, int16_t y, uint8_t color);
void Game_DispDrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);

#endif

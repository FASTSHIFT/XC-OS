#include "GamePrivate.h"
#include "SdFat.h"
#include "GameDef.h"

PageManager GamePage(GAME_MAX);

static bool ButtonState_Grp[MAX_BUTTON];

static uint8_t GameDispBuff[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT / 8];

static int16_t GameDispBaseX;
static int16_t GameDispBaseY;

bool Game_GetButtonPressed(uint8_t btn_id)
{
    return (btn_id >= MAX_BUTTON) ? false : ButtonState_Grp[btn_id];
}

void Game_SetButtonState(uint8_t btn_id, bool state)
{
    if(btn_id < MAX_BUTTON)
    {
        ButtonState_Grp[btn_id] = state;
    }
}

void Game_ClearButtonState()
{
    memset(ButtonState_Grp, 0, sizeof(ButtonState_Grp));
}

#define GAME_SAVED_PATH "//Game/"
#define GAME_SAVED_NAME "/save1.bin"

bool Game_WriteDataToFile(void* data, uint32_t len, const char* name)
{
    bool retval = false;
    SdFile file;

    String folder = GAME_SAVED_PATH + String(name);
    String path = folder + GAME_SAVED_NAME;

    if(!SD.exists(folder.c_str()))
    {
        SD.mkdir(folder.c_str());
    }

    if(file.open(path.c_str(), O_WRONLY | O_CREAT))
    {
        file.seekSet(0);
        file.write(data, len);
        file.close();
        retval = true;
    }
    return retval;
}

bool Game_ReadDataFromFile(void* data, uint32_t len, const char* name)
{
    bool retval = false;
    File file;
    String path = GAME_SAVED_PATH + String(name) + GAME_SAVED_NAME;

    file = SD.open(path, O_RDONLY);
    if(file)
    {
        file.seekSet(0);
        file.read(data, len);
        file.close();
        retval = true;
    }
    return retval;
}

#define GAME_REG(name)\
do{\
    extern void GameRegister_##name(uint8_t gameID);\
    GameRegister_##name(GAME_##name);\
}while(0)

void Game_Init()
{
    GAME_REG(MicroCity);

    GamePage.PageChangeTo(GAME_MicroCity);
    
    Game_DispGetContPos(&GameDispBaseX, &GameDispBaseY);
}

static volatile bool DispUpdating = false;

bool Game_DispBusy()
{
    return DispUpdating;
}

bool Game_DispReqUpdate()
{
    if(DispUpdating)
    {
        return false;
    }

    DispUpdating = true;
    return true;
}

void Game_DispTaskUpdate()
{
    if(!DispUpdating)
        return;
//    screen.setAddrWindow(BaseX, BaseY, BaseX + GAME_SCREEN_WIDTH - 1, GAME_SCREEN_HEIGHT + BaseY - 1);

    DISP_CS_CLR;
    for(int16_t y = 0; y < GAME_SCREEN_HEIGHT; y++)
    {
        for(int16_t x = 0; x < GAME_SCREEN_WIDTH; x++ )
        {
            uint8_t row = (uint8_t)y / 8;
#define IsWhite GameDispBuff[(row * GAME_SCREEN_WIDTH) + (uint8_t)x] &  _BV((uint8_t)y % 8)
            uint16_t color = IsWhite ? screen.White : screen.Black;

            int16_t x0 = (GameDispBaseX + x * 2);
            int16_t y0 = (GameDispBaseY + y * 2);
            DISP_SET_WINDOW(x0, y0, (x0 + 1), (y0 + 1));

            DISP_RS_SET;
            DISP_SEND(color);
            DISP_SEND(color);
            DISP_SEND(color);
            DISP_SEND(color);
            //screen.fillRect(BaseX + x * 2, BaseY + y * 2, 2, 2, IsWhite ? screen.White : screen.Black);
        }
    }
    DISP_CS_SET;

    DispUpdating = false;
}

uint8_t* Game_DispGetBuffer()
{
    return GameDispBuff;
}

void Game_DispDrawPixel(int16_t x, int16_t y, uint8_t color)
{
    if (x < 0 || x > (GAME_SCREEN_WIDTH - 1) || y < 0 || y > (GAME_SCREEN_HEIGHT - 1))
    {
        return;
    }

    uint8_t row = (uint8_t)y / 8;
    if (color)
    {
        GameDispBuff[(row * GAME_SCREEN_WIDTH) + (uint8_t)x] |=   _BV((uint8_t)y % 8);
    }
    else
    {
        GameDispBuff[(row * GAME_SCREEN_WIDTH) + (uint8_t)x] &= ~ _BV((uint8_t)y % 8);
    }
}

#define GAME_SCREEN_WHITE 1
#define GAME_SCREEN_BLACK 0

void Game_DispDrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color)
{
    // no need to dar at all of we're offscreen
    if (x + w < 0 || x > GAME_SCREEN_WIDTH - 1 || y + h < 0 || y > GAME_SCREEN_HEIGHT - 1)
        return;

    uint8_t* sBuffer = GameDispBuff;

    int yOffset = abs(y) % 8;
    int sRow = y / 8;
    if (y < 0)
    {
        sRow--;
        yOffset = 8 - yOffset;
    }
    int rows = h / 8;
    if (h % 8 != 0) rows++;
    for (int a = 0; a < rows; a++)
    {
        int bRow = sRow + a;
        if (bRow > (GAME_SCREEN_HEIGHT / 8) - 1) break;
        if (bRow > -2)
        {
            for (int iCol = 0; iCol < w; iCol++)
            {
                if (iCol + x > (GAME_SCREEN_WIDTH - 1)) break;
                if (iCol + x >= 0)
                {
                    if (bRow >= 0)
                    {
                        if      (color == GAME_SCREEN_WHITE) sBuffer[ (bRow * GAME_SCREEN_WIDTH) + x + iCol ] |= pgm_read_byte(bitmap + (a * w) + iCol) << yOffset;
                        else if (color == GAME_SCREEN_BLACK) sBuffer[ (bRow * GAME_SCREEN_WIDTH) + x + iCol ] &= ~(pgm_read_byte(bitmap + (a * w) + iCol) << yOffset);
                        else                                 sBuffer[ (bRow * GAME_SCREEN_WIDTH) + x + iCol ] ^= pgm_read_byte(bitmap + (a * w) + iCol) << yOffset;
                    }
                    if (yOffset && bRow < (GAME_SCREEN_HEIGHT / 8) - 1 && bRow > -2)
                    {
                        if      (color == GAME_SCREEN_WHITE) sBuffer[ ((bRow + 1)*GAME_SCREEN_WIDTH) + x + iCol ] |= pgm_read_byte(bitmap + (a * w) + iCol) >> (8 - yOffset);
                        else if (color == GAME_SCREEN_BLACK) sBuffer[ ((bRow + 1)*GAME_SCREEN_WIDTH) + x + iCol ] &= ~(pgm_read_byte(bitmap + (a * w) + iCol) >> (8 - yOffset));
                        else                                 sBuffer[ ((bRow + 1)*GAME_SCREEN_WIDTH) + x + iCol ] ^= pgm_read_byte(bitmap + (a * w) + iCol) >> (8 - yOffset);
                    }
                }
            }
        }
    }
}

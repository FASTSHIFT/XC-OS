#include "APP/Game/Arduboy2/Arduboy2.h"
#include "APP/Game/GamePrivate.h"
#include "Draw.h"
#include "Interface.h"
#include "Game.h"
#include "Simulation.h"

static Arduboy2Base arduboy;

uint8_t MicroCity::GetInput()
{
    uint8_t result = 0;

    if(arduboy.pressed(A_BUTTON))
    {
        result |= INPUT_A;
    }
    if(arduboy.pressed(B_BUTTON))
    {
        result |= INPUT_B;
    }
    if(arduboy.pressed(UP_BUTTON))
    {
        result |= INPUT_UP;
    }
    if(arduboy.pressed(DOWN_BUTTON))
    {
        result |= INPUT_DOWN;
    }
    if(arduboy.pressed(LEFT_BUTTON))
    {
        result |= INPUT_LEFT;
    }
    if(arduboy.pressed(RIGHT_BUTTON))
    {
        result |= INPUT_RIGHT;
    }

    return result;
}

void MicroCity::PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
    arduboy.drawPixel(x, y, colour);
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

void MicroCity::DrawBitmap(const uint8_t* bmp, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    arduboy.drawBitmap(x, y, bmp, w, h, WHITE);
}

void MicroCity::SaveCity()
{
    uint16_t address = EEPROM_STORAGE_SPACE_START;

    // Add a header so we know that the EEPROM contains a saved city
    arduboy.EEPROM.update(address++, 'C');
    arduboy.EEPROM.update(address++, 'T');
    arduboy.EEPROM.update(address++, 'Y');
    arduboy.EEPROM.update(address++, '1');

    uint8_t* ptr = (uint8_t*) &State;
    for(size_t n = 0; n < sizeof(GameState); n++)
    {
        arduboy.EEPROM.update(address++, *ptr);
        ptr++;
    }
}

bool MicroCity::LoadCity()
{
    uint16_t address = EEPROM_STORAGE_SPACE_START;

    if(arduboy.EEPROM.read(address++) != 'C') return false;
    if(arduboy.EEPROM.read(address++) != 'T') return false;
    if(arduboy.EEPROM.read(address++) != 'Y') return false;
    if(arduboy.EEPROM.read(address++) != '1') return false;

    uint8_t* ptr = (uint8_t*) &State;
    for(size_t n = 0; n < sizeof(GameState); n++)
    {
        *ptr = arduboy.EEPROM.read(address++);
        ptr++;
    }

    return true;
}

uint8_t* GetPowerGrid()
{
    return arduboy.getBuffer();
}

static void setup()
{
    arduboy.EEPROM.begin(GAME_FILE_PATH"MicroCity", "save.bin");
    
    GAME_SETUP_ONCE();

    arduboy.boot();
    arduboy.flashlight();
    arduboy.systemButtons();
    arduboy.bootLogo();
    arduboy.setFrameRate(25);

    MicroCity::InitGame();
}

static void loop()
{
    if(arduboy.nextFrame())
    {
        MicroCity::TickGame();
        arduboy.display(false);
    }
}

static void exit()
{
    arduboy.EEPROM.end();
}

void GameRegister_MicroCity(uint8_t gameID)
{
    GamePage.PageRegister(gameID, setup, loop, exit, NULL);
}

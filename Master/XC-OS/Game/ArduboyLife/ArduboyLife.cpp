// -------------------------------------
// Conway's Game Of Life for the Arduboy
// -------------------------------------

// Version 2.0

/*
Copyright (c) 2015-2019 Scott Allen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <APP/Game/Arduboy2/Arduboy2.h>
#include <APP/Game/GamePrivate.h>
#include <limits.h>

// Width and height of the screen in pixels (one pixel per cell)
#define LIFEWIDTH WIDTH
#define LIFEHEIGHT HEIGHT // must be a multiple of 8

// Button mappings
#define BTN_NEW RIGHT_BUTTON
#define BTN_REPLAY LEFT_BUTTON
#define BTN_SLOWER DOWN_BUTTON
#define BTN_FASTER UP_BUTTON
#define BTN_PAUSE A_BUTTON
#define BTN_RUN B_BUTTON

static constexpr unsigned long toneDuration = 2100; // max tone duration in ms

static constexpr int debounceWait = 50; // delay for button debounce

// Pointer to the screen buffer as a two dimensional array,
// the way the Life engine wants it.
static uint8_t (*grid)[LIFEWIDTH];

static unsigned long stepCount = 0;
static bool paused = false;

static unsigned int liveCells;

static unsigned int speedDly = 64;
static unsigned char speedNum = 6;

static unsigned long toneStartTime;
static bool tonePlaying = false;
static uint16_t lastToneCount = 0;

static long randSeed = 42; // The answer to life, the universe and everything...

static Arduboy2 arduboy;
static BeepPin1 beep;

static void newGame(uint8_t button);
static void pauseGame();
static void runGame();
static void goSlower();
static void goFaster();
static void genGrid(long seed);
static void nextStep();
static void showHelp();
static void showInfo();
static unsigned int countCells();
static void playTone();
static void toneTimer();
static bool waitRelease(uint8_t button);
static void delayPoll(unsigned long msDelay);
static unsigned int lifeIterate(uint8_t grid[][LIFEWIDTH]);
static uint8_t lifeByte(unsigned int left, unsigned int centre, unsigned int right);

// Handler for NEW and REPLAY game buttons
static void newGame(uint8_t button)
{
    genGrid(randSeed);
    waitRelease(button);
    delayPoll(1000);
}

// Handler for PAUSE / STEP button.
// Will step if paused.
static void pauseGame()
{
    if (paused)
    {
        nextStep();
    }
    else
    {
        paused = true;
    }
    waitRelease(BTN_PAUSE);
}

// Handler for RUN button
static void runGame()
{
    paused = false;
    waitRelease(BTN_RUN);
}

// Handler for the DECREASE SPEED button.
// Decrements the speed number and doubles the current speed limiting delay,
// up to a maximum of about 2 seconds.
// If both speed buttons are pressed, toggle sound and
// leave the speed unchanged.
static void goSlower()
{
    unsigned int lastspeedDly = speedDly;
    unsigned char lastSpeedNum = speedNum;

    if (speedDly < 16)
    {
        speedDly = 16;
        speedNum = 8;
    }
    else if (speedDly <= 1024)
    {
        speedDly *= 2;
        speedNum--;
    }

    if (waitRelease(BTN_SLOWER) == false)
    {
        speedNum = lastSpeedNum;
        speedDly = lastspeedDly;
        arduboy.audio.toggle();
        waitRelease(BTN_SLOWER + BTN_FASTER);
    }
}

// Handler for the INCREASE SPEED button.
// Increments the speed number and halves the current speed limiting delay,
// down to the minimum possible (no extra delay).
// If both speed buttons are pressed, toggle sound and
// leave the speed unchanged.
static void goFaster()
{
    unsigned int lastspeedDly = speedDly;
    unsigned char lastSpeedNum = speedNum;

    if (speedDly == 16)
    {
        speedDly = 0;
        speedNum = 9;
    }
    else if (speedDly > 0)
    {
        speedDly /= 2;
        speedNum++;
    }

    if (waitRelease(BTN_FASTER) == false)
    {
        speedNum = lastSpeedNum;
        speedDly = lastspeedDly;
        arduboy.audio.toggle();
        waitRelease(BTN_FASTER + BTN_SLOWER);
    }
}

// Generate a new game grid.
// Places a random number of random characters
// at random positions on the screen.
static void genGrid(long seed)
{
    int numChars;

    arduboy.clear();
    randomSeed(seed);
    numChars = random(10, 100);
    for (int c = 0; c <= numChars; c++)
    {
        arduboy.drawChar(random(LIFEWIDTH - 1 - 6), random(LIFEHEIGHT - 1 - 8),
                         random(1, 255), 1, 0, 1);
    }
    arduboy.display();
    stepCount = 0;
    liveCells = countCells();
    if (arduboy.audio.enabled())
    {
        lastToneCount = 0;
        playTone();
    }
}

// Iterate and display the next generation
static void nextStep()
{
    liveCells = lifeIterate(grid);
    arduboy.display();
    stepCount++;
    if (arduboy.audio.enabled())
    {
        playTone();
    }
}

// Display the button help screen
static void showHelp()
{
    arduboy.clear();

    arduboy.drawChar(47, 11, 0x1f, 1, 0, 1);
    arduboy.drawChar(43, 15, 0x10, 1, 0, 1);
    arduboy.drawChar(47, 19, 0x1e, 1, 0, 1);
    arduboy.drawChar(51, 15, 0x11, 1, 0, 1);

    arduboy.setCursor(32, 3);
    arduboy.print(F("FASTER"));
    arduboy.setCursor(6, 15);
    arduboy.print(F("REPLAY"));
    arduboy.setCursor(58, 15);
    arduboy.print(F("NEW"));
    arduboy.setCursor(32, 27);
    arduboy.print(F("SLOWER"));

    arduboy.drawFastHLine(69, 6, 19, 1);
    arduboy.drawFastHLine(69, 30, 19, 1);
    arduboy.drawFastVLine(88, 6, 25, 1);
    arduboy.drawFastHLine(89, 18, 5, 1);

    arduboy.setCursor(96, 15);
    arduboy.print(F("SOUND"));

    arduboy.setCursor(92, 42);
    arduboy.print(F("RUN"));
    arduboy.setCursor(28, 53);
    arduboy.print(F("PAUSE / STEP"));

    arduboy.fillCircle(115, 45, 4, 1);
    arduboy.fillCircle(105, 56, 4, 1);

    arduboy.display();
}

// Display the game status.
// Leave the screen buffer unaltered.
static void showInfo()
{
    uint8_t saveBuf[LIFEWIDTH * 3]; // screen buffer save area

    memcpy(saveBuf, grid, sizeof saveBuf);
    memset(grid, 0, sizeof saveBuf);

    arduboy.setCursor(0, 0);
    arduboy.print(F("Speed "));
    arduboy.print(speedNum);
    arduboy.setCursor(72, 0);
    arduboy.print(F("Sound "));
    if (arduboy.audio.enabled())
    {
        arduboy.println(F("ON"));
    }
    else
    {
        arduboy.println(F("OFF"));
    }
    arduboy.print(F("Cells "));
    arduboy.print(liveCells);
    arduboy.setCursor(84, 8);
    if (paused)
    {
        arduboy.println(F("PAUSED"));
    }
    else
    {
        arduboy.println(F("RUNNING"));
    }
    arduboy.print(F("Step  "));
    arduboy.print(stepCount);

    arduboy.display();
    memcpy(grid, saveBuf, sizeof saveBuf);
}

// Return a count of the number of live cells on the current grid
static unsigned int countCells()
{
    unsigned int total = 0;
    unsigned char *c = (unsigned char *) grid;

    for (unsigned int i = 0; i < (LIFEWIDTH * LIFEHEIGHT / 8); i++, c++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            if ((*c & (1 << j)) != 0)
            {
                total++;
            }
        }
    }
    return total;
}

// Play a tone with a frequency based on the number of live cells.
// The more cells, the higher the pitch.
static void playTone()
{
    uint16_t newToneCount;

    if ((newToneCount = (uint16_t)(1000000L / (liveCells * 3 + 20)) - 1) !=
            lastToneCount)
    {
        beep.tone(newToneCount);
        lastToneCount = newToneCount;
        toneStartTime = millis();
        tonePlaying = true;
    }
}

// Stop a playing tone if the maximum duration has elapsed
static void toneTimer()
{
    if (tonePlaying && (millis() - toneStartTime >= toneDuration))
    {
        beep.noTone();
        tonePlaying = false;
    }
}

// Show status information then wait for button release,
// including a simple delay based debounce.
//
// If, while waiting for the Slower or Faster button,
// the other speed button is also pressed, return false.
// In this case another call to this function is required
// to wait for both buttons to be released.
// Otherwise, return true.
static bool waitRelease(uint8_t button)
{
    showInfo();
    arduboy.delayShort(debounceWait);
    while (arduboy.notPressed(button) == false)
    {
        if (((button == BTN_SLOWER) || (button == BTN_FASTER)) &&
                arduboy.pressed(BTN_SLOWER + BTN_FASTER))
        {
            return false;
        }
        toneTimer();
    }
    arduboy.delayShort(debounceWait);
    arduboy.display();
    return true;
}

// Delay for the specified number of miliseconds or
// until a button is pressed
static void delayPoll(unsigned long msDelay)
{
    unsigned long waitTime = millis() + msDelay;

    while (((long) (millis() - waitTime) < 0) && (arduboy.buttonsState() == 0)) {}
}

//------------------------------------------------------------
// The Life engine
//
// Updates the provided grid with the next generation.
//
// Returns the number of live cells in the new grid.
//
// Only the torus type finite grid is implemented. (Left edge joins to the
// right edge and top edge joins to the bottom edge.)
//
// The grid is mapped as horizontal rows of bytes where each byte is a
// vertical line of cells with the least significant bit being the top cell.

// LIFEWIDTH and LIFEHEIGHT must have been previously defined.
// LIFEWIDTH is the width of the grid.
// LIFEHEIGHT is the height of the grid and must be a multiple of 8.
#define LIFELINES (LIFEHEIGHT / 8)
#define LIFEHIGHROW (LIFELINES - 1)
#define LIFEHIGHCOL (LIFEWIDTH - 1)

// number of bits in values from 0 to 7
static const unsigned char bitCount[] = { 0, 1, 1, 2, 1, 2, 2, 3 };

static unsigned int lifeCellCount; // Total number of live cells

static unsigned int lifeIterate(uint8_t grid[][LIFEWIDTH])
{
    uint8_t cur[LIFELINES][LIFEWIDTH]; // working copy of the current grid

    unsigned char row, col; // current row & column numbers
    unsigned char rowA, rowB, colR; // row above, row below, column to the right
    unsigned int left, centre, right; // packed vertical cell groups

    memcpy(cur, grid, sizeof cur);

    lifeCellCount = 0;

    rowA = LIFEHIGHROW;
    rowB = 1;
    for (row = 0; row < LIFELINES ; row++)
    {
        left = (((unsigned int) (cur[rowA][LIFEHIGHCOL])) >> 7) |
               (((unsigned int) cur[row][LIFEHIGHCOL]) << 1) |
               (((unsigned int) cur[rowB][LIFEHIGHCOL]) << 9);

        centre = (((unsigned int) (cur[rowA][0])) >> 7) |
                 (((unsigned int) cur[row][0]) << 1) |
                 (((unsigned int) cur[rowB][0]) << 9);

        colR = 1;
        for (col = 0; col < LIFEWIDTH; col++)
        {
            right = (((unsigned int) (cur[rowA][colR])) >> 7) |
                    (((unsigned int) cur[row][colR]) << 1) |
                    (((unsigned int) cur[rowB][colR]) << 9);

            grid[row][col] = lifeByte(left, centre, right);

            left = centre;
            centre = right;

            colR = (colR < LIFEHIGHCOL) ? colR + 1 : 0;
        }
        rowA = (rowA < LIFEHIGHROW) ? rowA + 1 : 0;
        rowB = (rowB < LIFEHIGHROW) ? rowB + 1 : 0;
    }
    return lifeCellCount;
}

// Calculate the next generation for 8 vertical cells (one byte of the
// array) that have been packet along with their neighbours into ints.
static uint8_t lifeByte(unsigned int left, unsigned int centre, unsigned int right)
{
    unsigned char count;
    uint8_t newByte = 0;

    for (unsigned char i = 0; i < 8; i++)
    {
        count = bitCount[left & 7] + bitCount[centre & 7] + bitCount[right & 7];

        if ((count == 3) || ((count == 4) && (centre & 2)))
        {
            newByte |= (1 << i);
            lifeCellCount++;
        }
        left >>= 1;
        centre >>= 1;
        right >>= 1;
    }
    return newByte;
}

static void setup()
{
    arduboy.EEPROM.begin(GAME_FILE_PATH"ArduboyLife", "save.bin");
    GAME_SETUP_ONCE();
    
    arduboy.begin();
    arduboy.audio.off();
    beep.begin();
    arduboy.clear();
    grid = (uint8_t (*)[LIFEWIDTH]) arduboy.getBuffer();

    // Display the intro screen
    arduboy.setTextSize(2);
    arduboy.setCursor(23, 2);
    arduboy.print(F("ARDUBOY"));
    arduboy.setTextSize(1);
    arduboy.setCursor(40, 23);
    arduboy.print(F("CONWAY'S"));
    arduboy.setCursor(43, 35);
    arduboy.print(F("Game of"));
    arduboy.setTextSize(2);
    arduboy.setCursor(41, 48);
    arduboy.print(F("LIFE"));
    arduboy.setTextSize(1);

    arduboy.display();
    delayPoll(3000);

    // Display the button help screen.
    // This will be the initial grid pattern unless
    // the NEW or REPLAY button is pressed.
    // Note that the REPLAY button won't restart this pattern.
    showHelp();
    liveCells = countCells();
    delayPoll(8000);

    arduboy.initRandomSeed();
}

static void loop()
{
    toneTimer();

    switch (arduboy.buttonsState())
    {
    case BTN_PAUSE:
        pauseGame();
        break;

    case BTN_RUN:
        runGame();
        break;

    case BTN_NEW:
        randSeed = random(LONG_MAX) + millis();
        newGame(BTN_NEW);
        break;

    case BTN_REPLAY:
        newGame(BTN_REPLAY);
        break;

    case BTN_SLOWER:
        goSlower();
        break;

    case BTN_FASTER:
        goFaster();
        break;
    }

    if (!paused)
    {
        nextStep();
    }

    if (speedDly != 0)
    {
        delayPoll(speedDly);
    }
}

static void exit()
{
    arduboy.EEPROM.end();
}

void GameRegister_ArduboyLife(uint8_t gameID)
{
    GamePage.PageRegister(gameID, setup, loop, exit, NULL);
}

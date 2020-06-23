#include "extend.h"
#include "GraphFont/GraphFont.cpp"

Arduboy2Ex ab;
GraphFont gf = GraphFont(ab.sBuffer, Arduboy2::width(), Arduboy2::height());

#include "AlertBox/AlertBox.h"
#include "StackArray/StackArray.h"
#include "Icons/Icons.h"

// I'm a heathen and I use public variables in my classes.
#include "Keypad/keypad.cpp"
#include "Function/function.cpp"
#include "Graph/graph.cpp"
#include "Table/table.cpp"
#include "APP/Game/GamePrivate.h"

static Keypad keypad;
static Function function;
Function directFunction;
static Graph graph;
static Table table;

static uint8_t state = 1;

static uint8_t cursor = 0;
static bool toTheTokens = false;

static bool redrawMenu = true;
static bool redrawGraph = true;
static bool justEntered = true;

static char funcText[32] = "5s(x)";
static char directText[32] = "2+2";

static uint8_t traceCursor;
static float directResult;

static void setup()
{
    ab.EEPROM.begin(GAME_FILE_PATH"ArduboyGraph2", "save.bin");
    GAME_SETUP_ONCE();
    ab.begin();

    function.getFunction(funcText);
    table.reset();
}

static void loop()
{
    if (!ab.nextFrame()) return;
    ab.pollButtons();

    if (redrawGraph)
    {
        ab.ditherScreen();
        ab.display();
        ab.clear();

        graph.draw(function);
        table.calculate(function);
        redrawGraph = false;
    }
    if (justEntered)
    {
        redrawMenu = true;
        // justEntered = false;
        // TODO: more efficient not-hacked-together menus
    }

    if (state == 0)
    {
        ab.clear();

        if (ab.justPressed(UP_BUTTON   ) && cursor > 0) cursor--;
        if (ab.justPressed(DOWN_BUTTON )) cursor++;
        if (ab.justPressed(LEFT_BUTTON )) toTheTokens = false;
        if (ab.justPressed(RIGHT_BUTTON)) toTheTokens = true;

        if (ab.justPressed(A_BUTTON))
        {
            if (toTheTokens)
            {
                function.function[cursor] = (uint8_t)keypad.lazyNumberEntry((float)function.function[cursor], 0);
            }
            else
            {
                function.constants[cursor] = keypad.lazyNumberEntry(function.constants[cursor], 3);
            }
        }
        if (ab.justPressed(B_BUTTON))
        {
            redrawGraph = true;
            redrawMenu = true;
            state = 1;
        }

        for (uint8_t i = 0; i < 8; i++)
        {
            gf.setCursor(0, i * 7);
            gf.print(function.constants[i]);

            gf.setCursor(64, i * 7);
            gf.print(function.function[i], HEX);
        }

        gf.setCursor(32, cursor * 7);
        gf.print(toTheTokens ? '>' : '<');
    }
    else if (state == 1)
    {
        ab.fastRect(64, 0, 64, 8, BLACK);

        if (ab.justPressed(UP_BUTTON) && cursor >= 2) cursor -= 2;
        if (ab.justPressed(DOWN_BUTTON) && cursor <= 3) cursor += 2;
        if (ab.justPressed(LEFT_BUTTON) && cursor > 0) cursor--;
        if (ab.justPressed(RIGHT_BUTTON) && cursor < 6) cursor++;

        if (ab.justPressed(A_BUTTON))
        {
            switch (cursor)
            {
            case 0:
                state = 2;
                break;
            case 1:
                state = 3;
                break;
            case 2:
                state = 4;
                break;
            case 4:
                state = 6;
                break;
            case 6:
                state = 0;
                break;
            }
            redrawMenu = true;
            cursor = 0;
        }

        if (redrawMenu)
        {
            ab.drawFastHLine(65, 0, 63);
            ab.drawFastHLine(64, 1, 64);
            ab.drawFastHLine(65, 63, 63);

            for (uint8_t i = 0; i < 2; i++)
            {
                ab.drawFastVLine(64 + i * 32, 2, 61);
            }
            for (uint8_t j = 0; j <= 3; j++)
            {
                ab.drawFastHLine(64, 2 + j * 20, 64);
            }

            gf.setCursor(66, 4);
            gf.print(F("Function"));
            gf.setCursor(98, 4);
            gf.print(F("Window"));
            gf.setCursor(66, 24);
            gf.print(F("Table"));
            gf.setCursor(98, 24);
            gf.print(F("Trace\ntodo"));
            gf.setCursor(66, 44);
            gf.print(F("Direct"));
            gf.setCursor(98, 44);
            gf.print(F("Tools\ntodo"));

            for (uint8_t i = 0; i < 6; i++)
            {
                Icons::drawIcon(i, 87 + (i % 2) * 32, 13 + (i / 2) * 20);
            }

            ab.drawFastHLine(65 + (cursor % 2) * 32, 2 + (cursor / 2) * 20, 31, BLACK);
            ab.drawFastHLine(65 + (cursor % 2) * 32, 22 + (cursor / 2) * 20, 31, BLACK);
            ab.drawFastHLine(65 + (cursor % 2) * 32, 1 + (cursor / 2) * 20, 32);
            ab.drawFastHLine(65 + (cursor % 2) * 32, 23 + (cursor / 2) * 20, 32);

            for (uint8_t i = 0; i < 3; i++)
            {
                ab.drawFastVLine(65 + i + (cursor % 2) * 32, 20 + i + (cursor / 2) * 20, 3 - i);
                ab.drawFastVLine(93 + i + (cursor % 2) * 32, 2 + (cursor / 2) * 20, i + 1);
            }

            redrawMenu = false;
        }
    }
    else if (state == 2)     // Function entry
    {
        ab.fastRect(64, 0, 64, 8, BLACK);

        if (justEntered)
        {
            keypad.x = 76;
            keypad.y = 12;
            keypad.begin(funcText);
            justEntered = false;
        }

        keypad.update();
        if (keypad.isSubmitted())
        {
            ErrorType error = function.getFunction(keypad.text);
            if (error == (ErrorType)0/*ErrorType::OK*/)
            {
                keypad.getValue(funcText);
                redrawGraph = true;
                justEntered = true;
                state = 1;
            }
            else
            {
                lazierShowError(error);
                justEntered = true; // lazy
            }
        }

        ab.drawRect(64, 0, 64, 11);
        gf.setCursor(66, 2);
        gf.println(keypad.text);

        keypad.draw();
    }
    else if (state == 3)     // Window
    {
        ab.fastRect(64, 0, 64, 8, BLACK);

        if (ab.justPressed(UP_BUTTON) && cursor > 0) cursor--;
        if (ab.justPressed(DOWN_BUTTON) && cursor < 3) cursor++;

        if (ab.justPressed(A_BUTTON))
        {
            switch (cursor)
            {
            case 0:
                graph.window.xMin = keypad.lazyNumberEntry(graph.window.xMin, 3);
                break;
            case 1:
                graph.window.xMax = keypad.lazyNumberEntry(graph.window.xMax, 3);
                break;
            case 2:
                graph.window.yMin = keypad.lazyNumberEntry(graph.window.yMin, 3);
                break;
            case 3:
                graph.window.yMax = keypad.lazyNumberEntry(graph.window.yMax, 3);
                break;
            }
            redrawGraph = true;
        }
        if (ab.justPressed(B_BUTTON))
        {
            redrawMenu = true;
            state = 1;
            cursor = 0;
        }

        gf.setCursor(72, 4);
        gf.print(F("Xmin"));
        gf.println(graph.window.xMin, 3);
        gf.print(F("Xmax"));
        gf.println(graph.window.xMax, 3);
        gf.print(F("Ymin"));
        gf.println(graph.window.yMin, 3);
        gf.print(F("Ymax"));
        gf.println(graph.window.yMax, 3);

        gf.setCursor(66, 4 + cursor * 7);
        gf.print('>');
    }
    else if (state == 4)     // Table
    {
        ab.fastRect(64, 0, 64, 8, BLACK);

        if (ab.justPressed(UP_BUTTON)) table.scrollUp(function);
        if (ab.justPressed(DOWN_BUTTON)) table.scrollDown(function);

        if (ab.justPressed(A_BUTTON))
        {
            table.deltaX = keypad.lazyNumberEntry(table.deltaX, 3);
            table.calculate(function);
        }
        if (ab.justPressed(B_BUTTON))
        {
            redrawMenu = true;
            state = 1;
            cursor = 0;
        }

        table.draw();
    }
    else if (state == 5)     // Trace
    {
        ab.fastRect(64, 0, 64, 8, BLACK);

        ab.fastRect(traceCursor, 0, 1, 8, INVERT);

        if (ab.justPressed(B_BUTTON))
        {
            ab.fastRect(traceCursor, 0, 1, 8, INVERT);
            redrawMenu = true;
            state = 1;
            cursor = 0;
        }
    }
    else if (state == 6)
    {
        keypad.lazyFunctionEntry(NULL);
        if (keypad.textLen)
        {
            ErrorType error = directFunction.getFunction(keypad.text);
            if (error == (ErrorType)0/*ErrorType::OK*/)
            {
                error = directFunction.calculate(0.0f, directResult);
                if (error == (ErrorType)0/*ErrorType::OK*/)
                {
                    state = 60;
                }
                else
                {
                    lazierShowError(error);
                }
            }
            else
            {
                lazierShowError(error);
            }
        }
        else
        {
            state = 1;
            redrawGraph = true;
            justEntered = true;
        }
    }
    else if (state == 60)
    {
        ab.clear();

        if (ab.justPressed(A_BUTTON))
        {
            state = 6;
        }
        if (ab.justPressed(B_BUTTON))
        {
            state = 1;
            redrawGraph = true;
            justEntered = true;
        }

        gf.setCursor(0, 0);
        gf.println(keypad.text);
        gf.println(directResult);
    }

    ab.display();
}

static void exit()
{
    ab.EEPROM.end();
}

void GameRegister_ArduboyGraph2(uint8_t gameID)
{
    GamePage.PageRegister(gameID, setup, loop, exit, NULL);
}

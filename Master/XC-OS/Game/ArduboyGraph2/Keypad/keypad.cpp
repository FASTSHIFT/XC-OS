#include "keypad.h"

void Keypad::begin(const char* function)
{
    // Set function mode.
    setLayout(1);

    // Reset submission status.
    submitted = false;

    // Clear existing text, if any.
    clear();

    // If we got a function...
    if (function != NULL)
    {
        // Copy string
        strcpy(text, function);
        // Get length.
        while (text[++textLen] != '\0');
    }
    else
    {
        // No length
        textLen = 0;
    }
}

void Keypad::begin(const float number = 0.0f, const uint8_t digits = 2)
{
    // Set number mode.
    setLayout(0);

    // Reset submission status.
    submitted = false;

    // Clear existing text, if any.
    clear();

    // Get string from number.
    dtostrf(number, 0, digits, text);
    // Get length.
    while (text[++textLen] != '\0');
}

// The lazy way to do things.
// Entirely takes over the loop. Don't use?
float Keypad::lazyNumberEntry(const float number = 0.0f, const uint8_t digits = 2)
{
    begin(number, digits);

    while (!submitted)
    {
        if (!ab.nextFrame()) continue;
        ab.pollButtons();

        ab.clear();

        update();
        draw();

        gf.setCursor(x + 2, y - 8);
        gf.print(text);

        ab.display();
    }

    return getValue();
}

// The lazy way to do things.
// Entirely takes over the loop. Don't use?
void Keypad::lazyFunctionEntry(char* function)
{
    begin(function);

    while (!submitted)
    {
        if (!ab.nextFrame()) continue;
        ab.pollButtons();

        ab.clear();

        update();
        draw();

        gf.setCursor(x + 2, y - 8);
        gf.print(text);

        ab.display();
    }

    if (function != NULL)
    {
        getValue(function);
    }
}

// The right way to do things.
void Keypad::update()
{
    if (ab.justPressed(LEFT_BUTTON ) && cursor > 0) cursor--;
    if (ab.justPressed(RIGHT_BUTTON) && cursor < (layout.width * layout.height) - 1) cursor++;

    if (ab.justPressed(UP_BUTTON   ) && cursor >= layout.width) cursor -= layout.width;
    if (ab.justPressed(DOWN_BUTTON ) && cursor < layout.width * (layout.height - 1)) cursor += layout.width;

    if (ab.justPressed(A_BUTTON)) press();
    if (ab.justPressed(B_BUTTON)) backspace();
}

// The right way to do things.
void Keypad::draw()
{
    // Keypad portion
    for (uint8_t i = 0; i <= layout.width; i++)
    {
        ab.drawFastVLine(x + i * 10, y, layout.height * 10 + 1);
    }
    for (uint8_t i = 0; i <= layout.height; i++)
    {
        ab.drawFastHLine(x, y + i * 10, layout.width * 10 + 1);
    }

    // Labels (tmp)
    char* keyString;
    for (uint8_t j = 0; j < layout.height; j++)
    {
        for (uint8_t i = 0; i < layout.width; i++)
        {
            keyString = getKeyString(getLayoutKey(i, j));
            gf.setCursor(x + i * 10 + 2 + (8 - gf.getCharsWidth(keyString)) / 2, y + j * 10 + 2);
            gf.print(keyString);
        }
    }

    // Cursor
    ab.drawRect(
        x + (cursor % layout.width) * 10,
        y + (cursor / layout.width) * 10,
        11, 11, BLACK
    );
    ab.drawRect(
        x + (cursor % layout.width) * 10 - 1,
        y + (cursor / layout.width) * 10 - 1,
        13, 13, WHITE
    );
}

// Because `submitted` is private.
bool Keypad::isSubmitted()
{
    return submitted;
}

// Get text
void Keypad::getValue(char* function)
{
    strcpy(function, text);
}

// Get value after everything is said and done.
float Keypad::getValue()
{
    return atof(text);
}

// Get layout
void Keypad::setLayout(const uint8_t index)
{
    cursor = index; //HACK

    memcpy_P(&layout, &KEYPAD_LAYOUTS[index], sizeof(KeypadLayout));
}

// Get layout key
uint8_t Keypad::getLayoutKey(const uint8_t i)
{
    return pgm_read_byte(&layout.labels[i]);
}
uint8_t Keypad::getLayoutKey(const uint8_t x, const uint8_t y)
{
    return getLayoutKey(x + y * layout.width);
}

// Get key label
char* Keypad::getKeyString(const uint8_t key)
{
    switch (key)
    {
    case KEYPAD_K_0:
        return (char*)KEYPAD_S_0;
    case KEYPAD_K_1:
        return (char*)KEYPAD_S_1;
    case KEYPAD_K_2:
        return (char*)KEYPAD_S_2;
    case KEYPAD_K_3:
        return (char*)KEYPAD_S_3;
    case KEYPAD_K_4:
        return (char*)KEYPAD_S_4;
    case KEYPAD_K_5:
        return (char*)KEYPAD_S_5;
    case KEYPAD_K_6:
        return (char*)KEYPAD_S_6;
    case KEYPAD_K_7:
        return (char*)KEYPAD_S_7;
    case KEYPAD_K_8:
        return (char*)KEYPAD_S_8;
    case KEYPAD_K_9:
        return (char*)KEYPAD_S_9;
    case KEYPAD_K_DECIMAL:
        return (char*)KEYPAD_S_DECIMAL;
    case KEYPAD_K_SIGN:
        return (char*)KEYPAD_S_SIGN;
    case KEYPAD_K_LPAREN:
        return (char*)KEYPAD_S_LPAREN;
    case KEYPAD_K_RPAREN:
        return (char*)KEYPAD_S_RPAREN;
    case KEYPAD_K_ADDITION:
        return (char*)KEYPAD_S_ADDITION;
    case KEYPAD_K_SUBTRACT:
        return (char*)KEYPAD_S_SUBTRACT;
    case KEYPAD_K_MULTIPLY:
        return (char*)KEYPAD_S_MULTIPLY;
    case KEYPAD_K_DIVISION:
        return (char*)KEYPAD_S_DIVISION;
    case KEYPAD_K_POWER:
        return (char*)KEYPAD_S_POWER;
    case KEYPAD_K_MODULUS:
        return (char*)KEYPAD_S_MODULUS;
    case KEYPAD_K_VAR:
        return (char*)KEYPAD_S_VAR;
    case KEYPAD_K_SINE:
        return (char*)KEYPAD_S_SINE;
    case KEYPAD_K_DELETE:
        return (char*)KEYPAD_S_DELETE;
    case KEYPAD_K_CLEAR:
        return (char*)KEYPAD_S_CLEAR;
    case KEYPAD_K_OK:
        return (char*)KEYPAD_S_OK;
    }
    return NULL;
}

// This is what happens when you press a button.
void Keypad::press()
{
    uint8_t key = getLayoutKey(cursor);

    switch (key)
    {
    case KEYPAD_K_OK: // OK
        submitted = true;
        text[textLen] = '\0';
        return;
    case KEYPAD_K_DELETE: // Backspace
        backspace();
        break;
    case KEYPAD_K_CLEAR: // Bomb
        clear();
        break;
    default:
        uint8_t i = 0;
        while (true)
        {
            text[textLen++] = getKeyString(key)[i++];
            if (getKeyString(key)[i] == '\0') break;
        }
        break;
    }
}

// When you backspace
void Keypad::backspace()
{
    if (textLen) text[--textLen] = '\0';
}
// When you backspace

// Delete all
void Keypad::clear()
{
    while (textLen) text[--textLen] = '\0';
}

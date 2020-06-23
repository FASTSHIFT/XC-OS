#pragma once

#define KEYPAD_K_0 0x00
#define KEYPAD_K_1 0x01
#define KEYPAD_K_2 0x02
#define KEYPAD_K_3 0x03
#define KEYPAD_K_4 0x04
#define KEYPAD_K_5 0x05
#define KEYPAD_K_6 0x06
#define KEYPAD_K_7 0x07
#define KEYPAD_K_8 0x08
#define KEYPAD_K_9 0x09
#define KEYPAD_K_DECIMAL 0x10
#define KEYPAD_K_SIGN 0x11
#define KEYPAD_K_LPAREN 0x12
#define KEYPAD_K_RPAREN 0x13
#define KEYPAD_K_ADDITION 0x14
#define KEYPAD_K_SUBTRACT 0x15
#define KEYPAD_K_MULTIPLY 0x16
#define KEYPAD_K_DIVISION 0x17
#define KEYPAD_K_POWER 0x18
#define KEYPAD_K_MODULUS 0x19
#define KEYPAD_K_SINE 0x20
// #define KEYPAD_K_E 0xFA
// #define KEYPAD_K_PI 0xFB
#define KEYPAD_K_VAR 0xFC
#define KEYPAD_K_DELETE 0xFD
#define KEYPAD_K_CLEAR 0xFE
#define KEYPAD_K_OK 0xFF

#ifndef NULL
#  define NULL 0
#endif

struct KeypadLayout
{
    uint8_t width;
    uint8_t height;
    const uint8_t* labels;
};

const uint8_t KEYPAD_LAYOUT_NUMBER[] PROGMEM =
{
    KEYPAD_K_OK, KEYPAD_K_DELETE, KEYPAD_K_CLEAR,
    KEYPAD_K_7, KEYPAD_K_8, KEYPAD_K_9,
    KEYPAD_K_4, KEYPAD_K_5, KEYPAD_K_6,
    KEYPAD_K_1, KEYPAD_K_2, KEYPAD_K_3,
    KEYPAD_K_0, KEYPAD_K_DECIMAL, KEYPAD_K_SIGN,
};

const uint8_t KEYPAD_LAYOUT_FUNCTION[] PROGMEM =
{
    KEYPAD_K_LPAREN, KEYPAD_K_OK, KEYPAD_K_DELETE, KEYPAD_K_CLEAR, KEYPAD_K_POWER,
    KEYPAD_K_RPAREN, KEYPAD_K_7, KEYPAD_K_8, KEYPAD_K_9, KEYPAD_K_DIVISION,
    KEYPAD_K_MODULUS, KEYPAD_K_4, KEYPAD_K_5, KEYPAD_K_6, KEYPAD_K_MULTIPLY,
    KEYPAD_K_SINE, KEYPAD_K_1, KEYPAD_K_2, KEYPAD_K_3, KEYPAD_K_SUBTRACT,
    KEYPAD_K_VAR, KEYPAD_K_0, KEYPAD_K_DECIMAL, KEYPAD_K_SIGN, KEYPAD_K_ADDITION,
};

const KeypadLayout KEYPAD_LAYOUTS[] PROGMEM =
{
    {3, 5, KEYPAD_LAYOUT_NUMBER},
    {5, 5, KEYPAD_LAYOUT_FUNCTION}
};

const char* const KEYPAD_S_0 PROGMEM = "0";
const char* const KEYPAD_S_1 PROGMEM = "1";
const char* const KEYPAD_S_2 PROGMEM = "2";
const char* const KEYPAD_S_3 PROGMEM = "3";
const char* const KEYPAD_S_4 PROGMEM = "4";
const char* const KEYPAD_S_5 PROGMEM = "5";
const char* const KEYPAD_S_6 PROGMEM = "6";
const char* const KEYPAD_S_7 PROGMEM = "7";
const char* const KEYPAD_S_8 PROGMEM = "8";
const char* const KEYPAD_S_9 PROGMEM = "9";
const char* const KEYPAD_S_DECIMAL PROGMEM = ".";
const char* const KEYPAD_S_SIGN PROGMEM = "-";
const char* const KEYPAD_S_LPAREN PROGMEM = "(";
const char* const KEYPAD_S_RPAREN PROGMEM = ")";
const char* const KEYPAD_S_ADDITION PROGMEM = "+";
const char* const KEYPAD_S_SUBTRACT PROGMEM = "-";
const char* const KEYPAD_S_MULTIPLY PROGMEM = "*";
const char* const KEYPAD_S_DIVISION PROGMEM = "/";
const char* const KEYPAD_S_POWER PROGMEM = "^";
const char* const KEYPAD_S_MODULUS PROGMEM = "%";
// const char* const KEYPAD_S_E PROGMEM = "E";
// const char* const KEYPAD_S_PI PROGMEM = "p";
const char* const KEYPAD_S_VAR PROGMEM = "x";
const char* const KEYPAD_S_SINE PROGMEM = "s(";
const char* const KEYPAD_S_DELETE PROGMEM = "<-";
const char* const KEYPAD_S_CLEAR PROGMEM = "<<";
const char* const KEYPAD_S_OK PROGMEM = "OK";

class Keypad
{
public:
    Keypad()
    {
        x = 12;
        y = 12;
        submitted = false;
    }
    void begin(const char* function = NULL);
    void begin(const float number, const uint8_t digits);

    void lazyFunctionEntry(char* function = NULL);
    float lazyNumberEntry(const float number, const uint8_t digits);

    void update();
    void draw();

    void getValue(char* function);
    float getValue();

    bool isSubmitted();

    char text[32];
    uint8_t textLen;

    uint8_t x;
    uint8_t y;

private:
    void setLayout(const uint8_t index);
    uint8_t getLayoutKey(const uint8_t i);
    uint8_t getLayoutKey(const uint8_t x, const uint8_t y);
    char* getKeyString(const uint8_t key);

    KeypadLayout layout;

    void press();

    void backspace();
    void clear();

    uint8_t cursor;

    bool submitted;
};

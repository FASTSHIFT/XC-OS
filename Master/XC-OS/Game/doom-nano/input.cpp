#include "FileGroup.h"
#include "input.h"
#include "constants.h"
#include "doom_nano.h"

uint8_t doomKey[K_MAX];

void input_setup()
{
}

bool input_left()
{
    return doomKey[K_LEFT];
    //return digitalRead(K_LEFT) == INPUT_STATE;
};

bool input_right()
{
    return doomKey[K_RIGHT];
    //return digitalRead(K_RIGHT) == INPUT_STATE;
};

bool input_up()
{
    return doomKey[K_UP];
    //return digitalRead(K_UP) == INPUT_STATE;
};

bool input_down()
{
    return doomKey[K_DOWN];
    //return digitalRead(K_DOWN) == INPUT_STATE;
};

bool input_fire()
{
    return doomKey[K_FIRE];
    //digitalRead(K_FIRE) == INPUT_STATE;
};

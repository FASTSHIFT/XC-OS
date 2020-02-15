#include "input.h"
#include "constants.h"
#include "doom_nano.h"
#include <stdint.h>
#include "APP/Game/GamePrivate.h"

void input_setup()
{
}

bool input_left()
{
    return Game_GetButtonPressed(LEFT_BUTTON);
};

bool input_right()
{
    return Game_GetButtonPressed(RIGHT_BUTTON);
};

bool input_up()
{
    return Game_GetButtonPressed(UP_BUTTON);
};

bool input_down()
{
    return Game_GetButtonPressed(DOWN_BUTTON);
};

bool input_fire()
{
    return Game_GetButtonPressed(A_BUTTON);
};

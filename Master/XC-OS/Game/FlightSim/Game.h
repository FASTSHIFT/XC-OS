#pragma once

#include <stdint.h>

class Game
{
public:
	static uint8_t globalTickFrame;

	static void Init();
	static void Tick();
	static void Draw();

private:

};

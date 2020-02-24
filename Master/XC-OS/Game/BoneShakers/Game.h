#pragma once

#include <stdint.h>
#include "Defines.h"

void InitGame(void);
void TickGame(void);
char GetMap(uint8_t x, uint8_t y);

// Implemented by platform
uint8_t GetInput(void);
void PutPixel(uint8_t x, uint8_t y, uint8_t colour);
void DrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap);
void DrawSolidBitmap(int16_t x, int16_t y, const uint8_t *bitmap);
void DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap,
	const uint8_t *mask, uint8_t frame, uint8_t mask_frame);
void SetLED(uint8_t r, uint8_t g, uint8_t b);
void FillScreen(uint8_t col);
//

struct Player
{
	int16_t x, z;
	int16_t oldX, oldZ;
	uint8_t angle;
	int8_t steering;
	int8_t turnVelocity;
	int8_t speed;
	int8_t acceleration;
	int16_t drawX, drawY;
	uint8_t frame;
	uint8_t lap;
	uint8_t lapProgress;
	uint8_t rank;
	int8_t endingRank;
};

enum class GameState : uint8_t
{
	Title,
	Begin,
	Play,
	Finished
};

extern Player players[NUM_PLAYERS];
extern GameState gameState;

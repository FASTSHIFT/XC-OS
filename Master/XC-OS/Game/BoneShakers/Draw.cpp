#include "Draw.h"
#include "Defines.h"
#include "Game.h"
#include "FixedMath.h"
#include "Font.h"

using namespace BoneShaker;

// Only include this once!
#include "LUT.h"
#include "Images.inc.h"

// a = y * vfov
// 
// z = h / tan(a)
// z = h / tan(y * vfov)
// 
// t = h / sin(a)
// 
// b = x * hfov
// u = t * sin(b)

const char readyStr[] PROGMEM =		"  READY  ";
const char setStr[] PROGMEM =		"   SET   ";
const char goStr[] PROGMEM =		"   GO!   ";
const char lap2Str[] PROGMEM =		"  LAP 2  ";
const char lap3Str[] PROGMEM =		"FINAL LAP";
const char wrongWayStr[] PROGMEM =	"WRONG WAY!";
const char firstPlaceStr[] PROGMEM =	"1ST PLACE!";
const char secondPlaceStr[] PROGMEM =	"2ND PLACE!";
const char thirdPlaceStr[] PROGMEM =	"3RD PLACE!";
const char fourthPlaceStr[] PROGMEM =	"4TH PLACE!";

const char* GetString(MessageType index)
{
	switch (index)
	{
	case MessageType::None:
		return nullptr;
	case MessageType::Ready:
		return readyStr;
	case MessageType::Set:
		return setStr;
	case MessageType::Go:
		return goStr;
	case MessageType::Lap2:
		return lap2Str;
	case MessageType::Lap3:
		return lap3Str;
	case MessageType::WrongWay:
		return wrongWayStr;
	case MessageType::FirstPlace:
		return firstPlaceStr;
	case MessageType::SecondPlace:
		return secondPlaceStr;
	case MessageType::ThirdPlace:
		return thirdPlaceStr;
	case MessageType::FourthPlace:
		return fourthPlaceStr;
	}

	return nullptr;
}

const char textures[] PROGMEM =
{
	// Light brick
	2, 2, 2, 2, 2, 2, 2, 2,
	2, 3, 3, 3, 3, 3, 3, 3,
	2, 3, 3, 3, 3, 3, 3, 3,
	2, 3, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 2, 3, 3, 3,
	3, 3, 3, 3, 2, 3, 3, 3,
	3, 3, 3, 3, 2, 3, 3, 3,

	/*1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,*/

	// ||
	4, 1, 1, 1, 1, 1, 1, 4,
	4, 2, 2, 2, 2, 2, 2, 4,
	0, 2, 2, 2, 2, 2, 2, 0,
	0, 2, 2, 2, 2, 2, 2, 0,
	4, 2, 2, 2, 2, 2, 2, 4,
	4, 1, 1, 1, 1, 1, 1, 4,
	0, 1, 1, 1, 1, 1, 1, 0,
	0, 1, 1, 1, 1, 1, 1, 0,

	// =
	4, 4, 0, 0, 4, 4, 0, 0,
	1, 2, 2, 2, 2, 1, 1, 1,
	1, 2, 2, 2, 2, 1, 1, 1,
	1, 2, 2, 2, 2, 1, 1, 1,
	1, 2, 2, 2, 2, 1, 1, 1,
	1, 2, 2, 2, 2, 1, 1, 1,
	1, 2, 2, 2, 2, 1, 1, 1,
	4, 4, 0, 0, 4, 4, 0, 0,

	// |-
	1, 4, 0, 0, 4, 4, 0, 0,
	4, 2, 2, 1, 1, 1, 1, 1,
	0, 2, 2, 2, 1, 1, 1, 1,
	0, 1, 2, 2, 2, 1, 1, 1,
	4, 1, 1, 2, 2, 2, 1, 1,
	4, 1, 1, 1, 2, 2, 2, 1,
	0, 1, 1, 1, 1, 2, 2, 2,
	0, 1, 1, 1, 1, 1, 2, 2,

	// -|
	4, 4, 0, 0, 4, 4, 0, 1,
	1, 1, 1, 1, 1, 2, 2, 4,
	1, 1, 1, 1, 2, 2, 2, 0,
	1, 1, 1, 2, 2, 2, 1, 0,
	1, 1, 2, 2, 2, 1, 1, 4,
	1, 2, 2, 2, 1, 1, 1, 4,
	2, 2, 2, 1, 1, 1, 1, 0,
	2, 2, 1, 1, 1, 1, 1, 0,

	// _|
	2, 2, 1, 1, 1, 1, 1, 4,
	2, 2, 2, 1, 1, 1, 1, 4,
	1, 2, 2, 2, 1, 1, 1, 0,
	1, 1, 2, 2, 2, 1, 1, 0,
	1, 1, 1, 2, 2, 2, 1, 4,
	1, 1, 1, 1, 2, 2, 2, 4,
	1, 1, 1, 1, 1, 2, 2, 0,
	4, 4, 0, 0, 4, 4, 0, 1,

	// |_
	4, 1, 1, 1, 1, 1, 2, 2,
	4, 1, 1, 1, 1, 2, 2, 2,
	0, 1, 1, 1, 2, 2, 2, 1,
	0, 1, 1, 2, 2, 2, 1, 1,
	4, 1, 2, 2, 2, 1, 1, 1,
	4, 2, 2, 2, 1, 1, 1, 1,
	0, 2, 2, 1, 1, 1, 1, 1,
	1, 4, 0, 0, 4, 4, 0, 0,

	// |-|
	4, 1, 1, 1, 1, 1, 1, 4,
	4, 2, 2, 2, 2, 2, 2, 4,
	4, 2, 2, 2, 2, 2, 2, 4,
	0, 2, 2, 2, 2, 2, 2, 0,
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 2, 2, 2, 2, 2, 2, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	0, 2, 2, 2, 2, 2, 2, 0,

	// Dark brick
	0, 0, 0, 0, 0, 1, 1, 1,
	0, 1, 1, 1, 0, 1, 1, 1,
	0, 1, 1, 1, 0, 1, 1, 1,
	0, 1, 1, 1, 0, 1, 1, 1,
	0, 1, 1, 1, 0, 0, 0, 0,
	0, 1, 1, 1, 0, 1, 1, 1,
	0, 1, 1, 1, 0, 1, 1, 1,
	0, 1, 1, 1, 0, 1, 1, 1,
};

const uint8_t patterns[] PROGMEM =
{
	0, 0,
	0, 0,

	1, 0,
	0, 0,

	1, 0,
	0, 1,

	1, 1,
	0, 1,

	1, 1,
	1, 1,
};

MessageType currentMessage = MessageType::None;
uint8_t messageTimer = 0;

uint8_t cameraAngle = 0;
int16_t cameraX = 0;
int16_t cameraZ = 0;

void ShowMessage(MessageType messageType)
{
	currentMessage = messageType;
	messageTimer = 40;
}

inline uint8_t GetPattern(uint8_t x, uint8_t y, uint8_t pattern)
{
	switch (pattern)
	{
	default:
	case 0:
		return 0;
		break;
	case 1:
		return (x & y) & 1;
		break;
	case 2:
		return (x ^ y) & 1;
		break;
	case 3:
		return (x | y) & 1;
		break;
	case 4:
		return 1;
		break;
	}
}

inline void GetFrameData(uint8_t frame, uint8_t distance, unsigned char const** outFrame, unsigned char const** outMask)
{
	switch (distance)
	{
		case 0:
		{
			switch (frame)
			{
			case 0:
			case 2:
				*outFrame = frame1_1;
				*outMask = frame1_1_mask;
				break;
			case 1:
				*outFrame = frame1_2;
				*outMask = frame1_2_mask;
				break;
			case 3:
				*outFrame = frame1_3;
				*outMask = frame1_3_mask;
				break;
			}
		}
		break;
		case 1:
		{
			switch (frame)
			{
			case 0:
			case 2:
				*outFrame = frame2_1;
				*outMask = frame2_1_mask;
				break;
			case 1:
				*outFrame = frame2_2;
				*outMask = frame2_2_mask;
				break;
			case 3:
				*outFrame = frame2_3;
				*outMask = frame2_3_mask;
				break;
			}
		}
		break;
		case 2:
		{
			switch (frame)
			{
			case 0:
			case 2:
				*outFrame = frame3_1;
				*outMask = frame3_1_mask;
				break;
			case 1:
				*outFrame = frame3_2;
				*outMask = frame3_2_mask;
				break;
			case 3:
				*outFrame = frame3_3;
				*outMask = frame3_3_mask;
				break;
			}
		}
		break;
		case 3:
		{
			*outFrame = frame4;
			*outMask = frame4_mask;
		}
		break;
	}
}

#define FOLLOW_PLAYER 0

void Render()
{
	if (gameState != GameState::Finished)
	{
		cameraX = players[FOLLOW_PLAYER].x - (int16_t)(((int32_t)FixedSin(players[FOLLOW_PLAYER].angle) * CAMERA_FOLLOW_DISTANCE) >> FIXED_SHIFT);
		cameraZ = players[FOLLOW_PLAYER].z - (int16_t)(((int32_t)FixedCos(players[FOLLOW_PLAYER].angle) * CAMERA_FOLLOW_DISTANCE) >> FIXED_SHIFT);
		cameraAngle = players[FOLLOW_PLAYER].angle;
	}
	//cameraX = players[0].x;
	//cameraZ = players[0].z;


	int16_t cameraFwdX = FixedSin(-cameraAngle);
	int16_t cameraFwdZ = FixedCos(-cameraAngle);
	int16_t cameraRightX = FixedSin(-cameraAngle + FIXED_ANGLE_90);
	int16_t cameraRightZ = FixedCos(-cameraAngle + FIXED_ANGLE_90);

	int y = DISPLAY_HEIGHT - 2;

	for (int x = -((2 * (int16_t)cameraAngle) & 63); x < DISPLAY_WIDTH; x += BACKGROUND_SIZE)
	{
		DrawBitmap(x, 0, background);
	}

	while (y > DISPLAY_HEIGHT / 3)
	{ 
		int16_t viewFwd = pgm_read_word(&viewFwdTable[y]);
		
		int16_t fwdRot = (int16_t)(((int32_t)cameraFwdZ * viewFwd) >> FIXED_SHIFT);
		int16_t rightRot = (int16_t)(((int32_t)cameraRightZ * viewFwd) >> FIXED_SHIFT);

		for (int x = 0; x < DISPLAY_WIDTH; x += 2)
		{
			int16_t viewX = pgm_read_word(&viewXTable[x]);
			int16_t viewRight = (int16_t)(((int32_t)viewX * viewFwd) >> FIXED_SHIFT);

			int16_t worldZ = fwdRot + (int16_t)(((int32_t)cameraFwdX * viewRight) >> FIXED_SHIFT) + cameraZ;
			int16_t worldX = (int16_t)(((int32_t)cameraRightX * viewRight) >> FIXED_SHIFT) + rightRot + cameraX;

			//int16_t worldZ = viewFwd + cameraZ;
			//int16_t worldX = viewRight + cameraX;

			uint8_t cellX = (uint8_t)(worldX >> FIXED_SHIFT);
			uint8_t cellZ = (uint8_t)(worldZ >> FIXED_SHIFT);

			uint8_t texU = (uint8_t)(worldX >> (FIXED_SHIFT - 3)) & 7;
			uint8_t texV = (uint8_t)(worldZ >> (FIXED_SHIFT - 3)) & 7;
			uint8_t texType = GetMap(cellX, cellZ);

			const char* texPtr = textures + TEXTURE_SIZE_BYTES * texType;

			{
				uint8_t patternType = pgm_read_byte(&texPtr[texV * 8 + texU]);

				//tex = ((tex >> 1) ^ ((x ^ y) & tex)) & 1;
				const uint8_t* patternPtr = patterns + patternType * 4;

				PutPixel(x, y, pgm_read_byte(patternPtr++));
				PutPixel(x + 1, y, pgm_read_byte(patternPtr++));
				PutPixel(x, y + 1, pgm_read_byte(patternPtr++));
				PutPixel(x + 1, y + 1, pgm_read_byte(patternPtr++));

				//PutPixel(x, y, GetPattern(x, y, pattern));
				//PutPixel(x + 1, y, GetPattern(x + 1, y, pattern));
				//PutPixel(x, y + 1, GetPattern(x, y + 1, pattern));
				//PutPixel(x + 1, y + 1, GetPattern(x + 1, y + 1, pattern));
			}
		}

		y-=2;
//		while (y >= 0)
//		{
//			PutPixel(x, y, 1);
//			PutPixel(x + 1, y, 1);
//			y--;
//		}
	}

	/*for (int y = 0; y < DISPLAY_HEIGHT; y++)
	{
		for (int x = 0; x < DISPLAY_WIDTH; x++)
		{
			PutPixel(x, y, 1);
		}
	}*/
//	players[0].frame++;
//
//	uint8_t frame = (players[0].frame >> 2) & 3;
	//frame = 0;

	cameraFwdX = FixedSin(cameraAngle);
	cameraFwdZ = FixedCos(cameraAngle);
	cameraRightX = FixedSin(cameraAngle + FIXED_ANGLE_90);
	cameraRightZ = FixedCos(cameraAngle + FIXED_ANGLE_90);

	uint8_t drawOrder[NUM_PLAYERS];
	uint8_t numVisibleSprites = 0;

	for (int n = 0; n < NUM_PLAYERS; n++)
	{
		Player* player = &players[n];

		//int16_t relX = player->x - cameraX;
		//int16_t relZ = player->z - cameraZ;
		//int16_t rotZ = ((cameraFwdZ * relZ) >> FIXED_SHIFT) + ((cameraFwdX * relX) >> FIXED_SHIFT);
		//int16_t rotX = ((cameraRightX * relX) >> FIXED_SHIFT) + ((cameraRightZ * relZ) >> FIXED_SHIFT);

		int16_t relX = player->x - cameraX;
		int16_t relZ = player->z - cameraZ;

		int16_t rotZ = (int16_t)(((int32_t)cameraFwdZ * relZ) >> FIXED_SHIFT) + (int16_t)(((int32_t)cameraFwdX * relX) >> FIXED_SHIFT);
		int16_t rotX = (int16_t)(((int32_t)cameraRightX * relX) >> FIXED_SHIFT) + (int16_t)(((int32_t)cameraRightZ * relZ) >> FIXED_SHIFT);

		if (rotZ < 1)
			continue;

		int16_t viewY = (int16_t)(((int32_t)FIXED_ONE * CAMERA_HEIGHT) / rotZ);
		int16_t outY = (int16_t)(((int32_t)viewY * (4 * DISPLAY_HEIGHT / 5)) >> FIXED_SHIFT) + (DISPLAY_HEIGHT / 5);

		if (outY < 22)
		{
			continue;
		}

		int16_t viewX = (int16_t)((FIXED_ONE * (int32_t)rotX) / rotZ);
		int16_t outX = (int16_t)(((int32_t)viewX * (2 * DISPLAY_WIDTH / 3)) >> FIXED_SHIFT) + (DISPLAY_WIDTH / 2);

		player->drawX = outX;
		player->drawY = outY;

		bool addedSprite = false;

		for (int v = 0; v < numVisibleSprites && !addedSprite; v++)
		{
			if (outY < players[drawOrder[v]].drawY)
			{
				for (int j = numVisibleSprites; j > v; j--)
				{
					drawOrder[j] = drawOrder[j - 1];
				}
				drawOrder[v] = n;
				numVisibleSprites++;
				addedSprite = true;
			}
		}

		if (!addedSprite)
		{
			drawOrder[numVisibleSprites] = n;
			numVisibleSprites++;
		}
	}

	for(int n = 0; n < numVisibleSprites; n++)
	{
		Player* player = &players[drawOrder[n]];

		if (player->drawX >= 0 && player->drawX < DISPLAY_WIDTH)
		{
			const uint8_t* frameData;
			const uint8_t* maskData;

			uint8_t distance = 0;
			uint8_t frame = (player->frame >> 2) & 3;

			if (player->steering < 0)
				frame = 3;
			else if (player->steering > 0)
				frame = 1;
			else if (player->acceleration == 0)
				frame = 0;

			if (player->drawY < 28)
			{
				distance = 3;
			}
			else if (player->drawY < 38)
			{
				distance = 2;
			}
			else if (player->drawY < 52)
			{
				distance = 1;
			}
			else
			{
				distance = 0;
			}

			GetFrameData(frame, distance, &frameData, &maskData);

			uint8_t frameWidth = pgm_read_byte(&frameData[0]);
			uint8_t frameHeight = pgm_read_byte(&frameData[1]);

			DrawSprite(player->drawX - (frameWidth >> 1), player->drawY - frameHeight, frameData, maskData, 0, 0);
			/*
			if (outY >= DISPLAY_HEIGHT)
			{
				outY = DISPLAY_HEIGHT - 1;
			}

			while (outY > 0)
			{
				PutPixel(outX, outY, 0);
				outY--;
			}*/
		}
	}

	//DrawInt(players[0].rank, 0, 0);
	
	if (currentMessage != MessageType::None)
	{
		DrawString(GetString(currentMessage), DISPLAY_WIDTH / 2 - 5 * 3, 5);
		if (messageTimer > 0)
		{
			messageTimer--;
		}
		else currentMessage = MessageType::None;
	}
}

const char pressButton[] PROGMEM = "Press A to start";
const char jameshhoward[] PROGMEM = "by @jameshhoward";

void DrawTitle()
{
	static uint8_t shake = 0;
	shake++;

	FillScreen(1);
	DrawSolidBitmap(25, 15, logo);
	DrawSolidBitmap(78, (shake >> 1) & 1, skull);
	DrawString(pressButton, 10, 40);
	DrawString(jameshhoward, 10, DISPLAY_HEIGHT - 7);
}

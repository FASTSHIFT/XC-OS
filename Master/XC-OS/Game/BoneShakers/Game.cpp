#include "Defines.h"
#include "Game.h"
#include "FixedMath.h"
#include "Draw.h"

const char map[MAP_WIDTH * MAP_HEIGHT] PROGMEM =
{
	0,0,0,0,0,0,0,0,0,0,8,8,8,8,8,8,
	0,3,2,2,2,2,2,2,4,0,0,0,0,0,0,0,
	0,1,0,0,0,0,0,0,1,0,0,3,2,2,4,0,
	0,1,0,8,8,8,8,0,6,2,2,5,0,0,1,0,
	0,1,0,0,0,8,8,0,0,0,0,0,0,0,1,0,
	0,6,2,4,0,8,8,8,8,8,8,8,8,0,1,0,
	0,0,0,1,0,8,8,8,0,0,0,0,0,0,1,0,
	8,8,0,7,0,8,8,8,0,3,2,2,2,2,5,0,
	8,8,0,1,0,8,8,8,0,1,0,0,0,0,0,0,
	8,8,0,1,0,8,8,8,0,6,2,2,2,4,0,8,
	8,8,0,1,0,8,8,8,0,0,0,0,0,1,0,8,
	8,8,0,1,0,8,8,8,8,8,8,8,0,1,0,8,
	8,8,0,1,0,8,8,8,8,0,0,0,0,1,0,8,
	8,8,0,1,0,0,0,0,0,0,3,2,2,5,0,8,
	8,8,0,6,2,2,2,2,2,2,5,0,0,0,0,8,
	8,8,0,0,0,0,0,0,0,0,0,0,8,8,8,8,
};

const uint8_t aiMap[] PROGMEM =
{
	2,3,3,3,3,3,3,3,3,3,8,8,8,8,8,8,
	2,2,2,2,2,2,2,2,3,4,2,3,3,3,3,4,
	2,1,1,1,1,1,1,2,3,3,2,2,2,2,3,4,
	2,1,4,8,8,8,8,2,2,2,2,1,1,2,3,4,
	2,1,4,4,4,8,8,2,1,1,1,1,1,2,3,4,
	2,1,4,4,4,8,8,8,8,8,8,8,8,2,3,4,
	2,1,1,1,4,8,8,8,3,3,3,3,3,3,3,4,
	8,8,2,1,4,8,8,8,2,3,4,4,4,4,4,4,
	8,8,2,1,4,8,8,8,2,3,3,3,3,3,3,3,
	8,8,2,1,4,8,8,8,2,2,2,2,2,3,4,8,
	8,8,2,1,4,8,8,8,2,1,1,1,2,3,4,8,
	8,8,2,1,4,8,8,8,8,8,8,8,2,3,4,8,
	8,8,2,1,4,8,8,8,8,3,3,3,3,3,4,8,
	8,8,2,1,4,3,3,3,3,3,3,4,4,4,4,8,
	8,8,2,1,4,4,4,4,4,4,4,4,1,1,4,8,
	8,8,1,1,1,1,1,1,1,1,1,1,8,8,8,8,
};

#define MAX_PROGRESS_VALUE 61

const uint8_t progressMap[] PROGMEM =
{
	 8, 8, 9,10,11,12,13,14,15,15,99,99,99,99,99,99,
	 8, 8, 9,10,11,12,13,14,15,15,21,21,22,23,24,24,
	 7, 7, 7,10,11,12,13,14,16,18,21,21,22,23,24,24,
	 6, 6, 6,99,99,99,99,17,17,18,19,20,20,25,25,25,
	 5, 5, 3, 2, 2,99,99,17,17,18,19,20,20,26,26,26,
	 4, 4, 3, 2, 2,99,99,99,99,99,99,99,99,27,27,27,
	 4, 4, 1, 1, 1,99,99,99,34,34,33,32,31,30,28,28,
	99,99, 0, 0, 0,99,99,99,34,34,33,32,31,30,29,29,
	99,99,61,61,61,99,99,99,35,35,37,38,39,40,29,29,
	99,99,60,60,60,99,99,99,36,36,37,38,39,40,40,99,
	99,99,59,59,59,99,99,99,36,36,37,38,39,41,41,99,
	99,99,58,58,58,99,99,99,99,99,99,99,42,42,42,99,
	99,99,57,57,57,99,99,99,99,47,47,46,45,43,43,99,
	99,99,56,56,56,53,52,51,50,49,47,46,45,44,44,99,
	99,99,55,55,54,53,52,51,50,49,48,46,45,44,44,99,
	99,99,55,55,54,53,52,51,50,49,48,46,99,99,99,99,
};


Player players[NUM_PLAYERS];
uint8_t playerPositions[NUM_PLAYERS];
GameState gameState = GameState::Title;
uint8_t gameTimer = 0;

// For rubber banding
const uint8_t maxRoadSpeeds[NUM_PLAYERS] PROGMEM = 
{
	MAX_ROAD_SPEED - 0,
	MAX_ROAD_SPEED + 10,
	MAX_ROAD_SPEED + 15,
	MAX_ROAD_SPEED + 20
};

void UpdatePlayers(void);
void SetStartPositions(void);

char GetMap(uint8_t x, uint8_t y)
{
	if (x < MAP_WIDTH && y < MAP_HEIGHT)
	{
		return pgm_read_byte(&map[y * MAP_WIDTH + x]);
	}

	return 8;
}

void SetStartPositions()
{
	int row = 0;
	int col = 0;

	for (int n = 0; n < NUM_PLAYERS; n++)
	{
		Player* player = &players[n];
		player->oldX = player->x = START_X * FIXED_ONE + (FIXED_ONE / 4) + col * (FIXED_ONE / 2);
		player->oldZ = player->z = START_Z * FIXED_ONE + (FIXED_ONE / 4) - row * (FIXED_ONE / 2);
		player->angle = FIXED_ANGLE_90 * 2;
		player->lap = 1;
		player->lapProgress = 0;
		player->endingRank = -1;
		player->steering = player->acceleration = player->speed = player->turnVelocity = player->rank = player->frame = 0;
		col++;
		if (col == 2)
		{
			col = 0;
			row++;
		}
	}
}

void InitGame()
{
	SetStartPositions();
	ShowMessage(MessageType::None);
}

void UpdateLocalPlayer()
{
	uint8_t input = GetInput();

	int8_t steering = 0;
	if (input & INPUT_LEFT)
		steering--;
	if (input & INPUT_RIGHT)
		steering++;
	players[0].steering = steering;
	players[0].acceleration = (input & INPUT_B) ? 1 : 0;

}

void TickGame()
{
	gameTimer++;

	switch (gameState)
	{
	case GameState::Title:
	{
		DrawTitle();

		if (GetInput() & (INPUT_A | INPUT_B))
		{
			InitGame();
			gameTimer = 0;
			gameState = GameState::Begin;
		}
		return;
	}
	break;
	case GameState::Begin:
	{
		if (gameTimer < 20)
		{
			SetLED(0, 0, 0);
		}
		else if (gameTimer < 40)
		{
			SetLED(1, 0, 0);
			ShowMessage(MessageType::Ready);
		}
		else if (gameTimer < 60)
		{
			SetLED(1, 1, 0);
			ShowMessage(MessageType::Set);
		}
		else
		{
			SetLED(0, 1, 0);
			gameState = GameState::Play;
			ShowMessage(MessageType::Go);
		}
	}
		break;
	case GameState::Play:
		if (gameTimer > 80)
		{
			SetLED(0, 0, 0);
		}
		UpdateLocalPlayer();
		UpdatePlayers();
		break;
	case GameState::Finished:
		{
			UpdatePlayers();

			switch (players[0].endingRank)
			{
			case 0:
				ShowMessage(MessageType::FirstPlace);
				break;
			case 1:
				ShowMessage(MessageType::SecondPlace);
				break;
			case 2:
				ShowMessage(MessageType::ThirdPlace);
				break;
			case 3:
				ShowMessage(MessageType::FourthPlace);
				break;
			}
			
			gameTimer++;
			if (gameTimer > 200)
			{
				gameState = GameState::Title;
			}
		}
		break;
	}

	Render();
}

bool IsBlocked(int16_t x, int16_t z)
{
	uint8_t cellX = (uint8_t)(x >> FIXED_SHIFT);
	uint8_t cellZ = (uint8_t)(z >> FIXED_SHIFT);

	if (cellX < MAP_WIDTH && cellZ < MAP_HEIGHT)
	{
		uint8_t cell = pgm_read_byte(&map[cellZ * MAP_WIDTH + cellX]);
		return cell == 8;
	}
	return true;
}

void UpdatePlayers()
{
	for (int n = 0; n < NUM_PLAYERS; n++)
	{
		Player* player = &players[n];

		uint8_t cellX = (uint8_t)(player->x >> FIXED_SHIFT);
		uint8_t cellZ = (uint8_t)(player->z >> FIXED_SHIFT);

		uint8_t lapProgress = pgm_read_byte(&progressMap[cellZ * MAP_WIDTH + cellX]);
		if (lapProgress != player->lapProgress)
		{
			if (lapProgress == 0 && player->lapProgress == MAX_PROGRESS_VALUE)
			{
				player->lap++;

				if (n == 0)
				{
					if (player->lap == 2)
					{
						ShowMessage(MessageType::Lap2);
					}
					else if (player->lap == 3)
					{
						ShowMessage(MessageType::Lap3);
					}
					else if (player->lap == 4)
					{
						gameState = GameState::Finished;
						gameTimer = 0;
					}
				}

				if (player->lap == 4 && player->endingRank == -1)
				{
					int8_t endingRank = -1;
					for (int x = 0; x < NUM_PLAYERS; x++)
					{
						if (players[x].endingRank > endingRank)
						{
							endingRank = players[x].endingRank;
						}
					}
					player->endingRank = endingRank + 1;
				}
			}
			else if (player->lapProgress == 0 && lapProgress == MAX_PROGRESS_VALUE && player->lap > 0)
			{
				player->lap--;
			}

			player->lapProgress = lapProgress;
		}

		// AI
		if (n > 0 || gameState == GameState::Finished)
		{
			uint8_t fwdCellX = (uint8_t)((player->x + FixedSin(player->angle) / 3) >> FIXED_SHIFT);
			uint8_t fwdCellZ = (uint8_t)((player->z + FixedCos(player->angle) / 3) >> FIXED_SHIFT);
			uint8_t aiCell = pgm_read_byte(&aiMap[fwdCellZ * MAP_WIDTH + fwdCellX]);
			uint8_t intendedAngle = player->angle;
			switch (aiCell)
			{
			case 3:
				intendedAngle = 0;
				break;
			case 2:
				intendedAngle = FIXED_ANGLE_90;
				break;
			case 1:
				intendedAngle = FIXED_ANGLE_90 * 2;
				break;
			case 4:
				intendedAngle = FIXED_ANGLE_90 * 3;
				break;
			}

			int16_t angleDiff = intendedAngle - player->angle;

			if (angleDiff > FIXED_ANGLE_90 * 2)
			{
				angleDiff -= FIXED_ANGLE_MAX;
			}
			if (angleDiff < FIXED_ANGLE_90 * -2)
			{
				angleDiff += FIXED_ANGLE_MAX;
			}

			if (angleDiff < -5)
			{
				player->steering = -1;
			}
			else if (angleDiff > 5)
			{
				player->steering = 1;
			}
			else
			{
				player->steering = 0;
			}

			//player->acceleration = (angleDiff > -32 && angleDiff < 32) ? 1 : 0;
			player->acceleration = (angleDiff > -48 && angleDiff < 48) ? 1 : 0;
		}

		if (player->steering == 0)
		{
			if (player->turnVelocity > 0)
			{
				player->turnVelocity--;
			}
			else if (player->turnVelocity < 0)
			{
				player->turnVelocity++;
			}
		}
		else if (player->steering < 0)
		{
			if (player->turnVelocity > -MAX_TURN_VELOCITY)
			{
				player->turnVelocity--;
			}
		}
		else if (player->steering > 0)
		{
			if (player->turnVelocity < MAX_TURN_VELOCITY)
			{
				player->turnVelocity++;
			}
		}

		uint8_t cell = map[cellZ * MAP_WIDTH + cellX];

		player->angle = FIXED_ANGLE_WRAP(player->angle + player->turnVelocity);
		int maxSpeed = cell == 0 ? MAX_SIDE_SPEED : (player->steering ? MAX_STEERING_SPEED : pgm_read_byte(&maxRoadSpeeds[player->rank]));

		if (player->acceleration > 0)
		{
			uint8_t frameDelta = player->speed >> 4;
			if (frameDelta == 0)
				frameDelta = 1;
			player->frame += frameDelta;

			if (player->speed < maxSpeed)
			{
				player->speed++;
			}
			else
			{
				player->speed--;
			}
		}
		else if (player->speed > 0)
		{
			player->speed--;
		}

		//int16_t velX = player->x - player->oldX;
		//int16_t velZ = player->z - player->oldZ;

		player->oldX = player->x;
		player->oldZ = player->z;

		//velX = (velX * 240) / 256;
		//velZ = (velZ * 240) / 256;

		player->x += (FixedSin(player->angle) * player->speed) >> 10;
		player->z += (FixedCos(player->angle) * player->speed) >> 10;
	}

	for (int n = 0; n < NUM_PLAYERS; n++)
	{
		Player* player = &players[n];
		for (int i = n + 1; i < NUM_PLAYERS; i++)
		{
			Player* other = &players[i];
			int16_t diffX = player->x - other->x;
			int16_t diffZ = player->z - other->z;

			if (diffX < 0) diffX = -diffX;
			if (diffZ < 0) diffZ = -diffZ;

			if (diffX < PLAYER_SIZE && diffZ < PLAYER_SIZE)
			{
				if (diffX > diffZ)
				{
					int16_t centerX = (player->x + other->x) >> 1;

					if (player->oldX < other->oldX)
					{
						player->x = centerX - PLAYER_SIZE;
						other->x = centerX + PLAYER_SIZE;
					}
					else
					{
						player->x = centerX + PLAYER_SIZE;
						other->x = centerX - PLAYER_SIZE;
					}
				}
				else
				{
					int16_t centerZ = (player->z + other->z) >> 1;

					if (player->oldZ < other->oldZ)
					{
						player->z = centerZ - PLAYER_SIZE;
						other->z = centerZ + PLAYER_SIZE;
					}
					else
					{
						player->z = centerZ + PLAYER_SIZE;
						other->z = centerZ - PLAYER_SIZE;
					}
				}
			}
		}
	}

	for (int n = 0; n < NUM_PLAYERS; n++)
	{
		Player* player = &players[n];

		if (player->x < player->oldX)
		{
			if (IsBlocked(player->x - PLAYER_SIZE, player->z))
			{
				player->x = ((player->x - PLAYER_SIZE) & 0xff00) + FIXED_ONE + PLAYER_SIZE;
			}
		}
		else if (player->x > player->oldX)
		{
			if (IsBlocked(player->x + PLAYER_SIZE, player->z))
			{
				player->x = ((player->x + PLAYER_SIZE) & 0xff00) - PLAYER_SIZE;
			}
		}

		if (player->z < player->oldZ)
		{
			if (IsBlocked(player->x, player->z - PLAYER_SIZE))
			{
				player->z = ((player->z - PLAYER_SIZE) & 0xff00) + FIXED_ONE + PLAYER_SIZE;
			}
		}
		else if (player->z > player->oldZ)
		{
			if (IsBlocked(player->x, player->z + PLAYER_SIZE))
			{
				player->z = ((player->z + PLAYER_SIZE) & 0xff00) - PLAYER_SIZE;
			}
		}

	}

	for (int p = 0; p < NUM_PLAYERS; p++)
	{
		bool foundPosition = false;

		for (int n = 0; n < p && !foundPosition; n++)
		{
			if (players[p].lap >= players[playerPositions[n]].lap && players[p].lapProgress > players[playerPositions[n]].lapProgress)
			{
				for (int i = p; i > n; i--)
				{
					playerPositions[i] = playerPositions[i - 1];
				}
				playerPositions[n] = p;
				foundPosition = true;
			}
		}

		if (!foundPosition)
		{
			playerPositions[p] = p;
		}
	}

	for (int n = 0; n < NUM_PLAYERS; n++)
	{
		players[playerPositions[n]].rank = n;
	}
}

/*
void UpdatePlayers()
{
	for (int n = 0; n < NUM_PLAYERS; n++)
	{
		Player* player = &players[n];

		if (player->steering == 0)
		{
			if (player->turnVelocity > 0)
			{
				player->turnVelocity--;
			}
			else if (player->turnVelocity < 0)
			{
				player->turnVelocity++;
			}
		}
		else if (player->steering < 0)
		{
			if (player->turnVelocity > -MAX_TURN_VELOCITY)
			{
				player->turnVelocity--;
			}
		}
		else if (player->steering > 0)
		{
			if (player->turnVelocity < MAX_TURN_VELOCITY)
			{
				player->turnVelocity++;
			}
		}

		uint8_t cellX = (uint8_t)(player->x >> FIXED_SHIFT);
		uint8_t cellZ = (uint8_t)(player->z >> FIXED_SHIFT);

		uint8_t cell = map[cellZ * MAP_WIDTH + cellX];

		player->angle = FIXED_ANGLE_WRAP(player->angle + player->turnVelocity);
		int maxSpeed = cell == 0 ? MAX_SIDE_SPEED : MAX_ROAD_SPEED;

		if (player->acceleration > 0)
		{
			if (player->speed < maxSpeed)
			{
				player->speed++;
			}
			else
			{
				player->speed--;
			}
		}
		else if (player->speed > 0)
		{
			player->speed--;
		}

		//int16_t velX = player->x - player->oldX;
		//int16_t velZ = player->z - player->oldZ;

		player->oldX = player->x;
		player->oldZ = player->z;

		//velX = (velX * 240) / 256;
		//velZ = (velZ * 240) / 256;

		player->x += (FixedSin(player->angle) * player->speed) >> 10;
		player->z += (FixedCos(player->angle) * player->speed) >> 10;
	}
}
*/

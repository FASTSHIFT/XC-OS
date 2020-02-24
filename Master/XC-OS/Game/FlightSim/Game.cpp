#include "Defines.h"
#include "Game.h"
#include "Platform.h"
#include "FixedMath.h"
#include <math.h>
#include <stdlib.h>
//#include <stdio.h>

using namespace FlightSim;

#include "Generated/TextureData.inc.h"

// When enabled will effectively render at quarter resolution
#define USE_COARSE_RENDERING 1

struct ScreenPoint
{
	int x, y;
	fixed16_t z;
};

struct Camera
{
	Vector3s position;
	Matrix3x3 rotation;
	Matrix3x3 invRotation;
	angle_t pitch, roll, yaw;
};

Camera camera;

void Game::Init()
{
	camera.rotation = Matrix3x3::Identity();
	camera.position = Vector3s(64 * 16, 75, 64 * 16);

	// White out areas for HUD
	for (int x = 0; x < 38; x++)
	{
		for (int y = 0; y < 20; y++)
		{
			Platform::PutPixel(x + 1, y + DISPLAY_HEIGHT - 21, COLOUR_WHITE);
			Platform::PutPixel(x + DISPLAY_WIDTH - 39, y + DISPLAY_HEIGHT - 21, COLOUR_WHITE);
		}
	}
}

ScreenPoint ProjectPoint(const Vector3s& inPoint)
{
	constexpr fixed16_t multiplier = 64.0f;

	ScreenPoint result;
	
	if (inPoint.z != 0)
	{
		fixed16_t w = multiplier / inPoint.z;
		result.x = DISPLAY_WIDTH / 2 + int(inPoint.x * w);
		result.y = DISPLAY_HEIGHT / 2 - int(inPoint.y * w);
		result.z = inPoint.z;
	}
	else
	{
		result.x = result.y = 0;
		result.z = 0;
	}

	return result;
}

Vector3s WorldToView(const Vector3s& inPoint)
{
	Vector3s worldPosition = inPoint - camera.position;
	return camera.invRotation * worldPosition;
}

const uint8_t ditherPattern1[] =
{
	1, 3,
	4, 2
};
const uint8_t ditherPattern2[] =
{
	4, 2,
	1, 3,
};

inline Vector3b Lerp(const Vector3b& a, const Vector3b& b, fixed8_t alpha)
{
	const fixed8_t oneMinusAlpha = fixed8_t::FromRaw(64 - alpha.rawValue);
	return Vector3b(
		fixed8_t::FromRaw((a.x.rawValue * oneMinusAlpha.rawValue + b.x.rawValue * alpha.rawValue) >> FixedFractionalBits),
		fixed8_t::FromRaw((a.y.rawValue * oneMinusAlpha.rawValue + b.y.rawValue * alpha.rawValue) >> FixedFractionalBits),
		fixed8_t::FromRaw((a.z.rawValue * oneMinusAlpha.rawValue + b.z.rawValue * alpha.rawValue) >> FixedFractionalBits));
}

struct DitherWriter
{
	DitherWriter(uint8_t* inBufferPtr, const uint8_t* inDitherPattern, uint8_t inMaxWrites) :
		bufferPtr(inBufferPtr),
		ditherPattern(inDitherPattern),
		writesLeft(inMaxWrites),
		buffer1(0),
		buffer2(0),
		bufferPos(0),
		writeMask(1)
	{
	}

	bool CanWrite()
	{
		return writesLeft > 0;
	}

	inline void Write(uint8_t outColour)
	{
		// Fill in 2x2 pixels based on dither pattern
		if (outColour >= ditherPattern[0])
		{
			buffer1 |= writeMask;
		}
		if (outColour >= ditherPattern[1])
		{
			buffer2 |= writeMask;
		}
		writeMask <<= 1;
		if (outColour >= ditherPattern[2])
		{
			buffer1 |= writeMask;
		}
		if (outColour >= ditherPattern[3])
		{
			buffer2 |= writeMask;
		}
		writeMask <<= 1;

		bufferPos++;
		if (bufferPos == 4)
		{
			// Write out buffer and move on to the next 8 pixels
			bufferPtr[0] = buffer1;
			bufferPtr[1] = buffer2;
			bufferPtr += DISPLAY_WIDTH;
			writeMask = 1;
			bufferPos = 0;
			buffer1 = 0;
			buffer2 = 0;
		}

		writesLeft--;
	}

	uint8_t* bufferPtr;
	const uint8_t* ditherPattern;
	uint8_t writesLeft;
	uint8_t buffer1, buffer2, bufferPos, writeMask;
};

template<int textureSize>
uint8_t ReadTextureFromWorldSpace(const uint8_t* texPtr, const fixed16_t worldX, const fixed16_t worldZ)
{
	// Convert to texture space
	uint8_t u = ((int)(worldX) >> 4) & (textureSize - 1);
	uint8_t v = ((int)(worldZ) >> 4) & (textureSize - 1);

	int index = v * textureSize + u;
	return pgm_read_byte(&texPtr[index]);
}

inline void DrawGroundSpan(DitherWriter& ditherWriter, const Vector3b& viewDirTop, const Vector3b& viewDirBottom, uint8_t& outY, uint8_t height)
{
	uint8_t steps = height >> 1;
	Vector3b lastWorldDir = Lerp(viewDirTop, viewDirBottom, fixed8_t::FromRaw(outY));
	fixed16_t lastDistance = FixedMath::QuickDivide(fixed16_t(camera.position.y), -lastWorldDir.y);
	fixed16_t lastIntersectionX = camera.position.x + lastWorldDir.x * lastDistance;
	fixed16_t lastIntersectionZ = camera.position.z + lastWorldDir.z * lastDistance;
	fixed8_t interpThreshold = fixed8_t::FromRaw(-6);

	while (steps > 0)
	{
		if (steps >= 4 && lastWorldDir.y < interpThreshold)
		{
			outY += 8;
			steps -= 4;

			Vector3b nextWorldDir = Lerp(viewDirTop, viewDirBottom, fixed8_t::FromRaw(outY));
			fixed16_t nextDistance = FixedMath::QuickDivide(fixed16_t(camera.position.y), -nextWorldDir.y);
			fixed16_t nextIntersectionX = camera.position.x + nextWorldDir.x * nextDistance;
			fixed16_t nextIntersectionZ = camera.position.z + nextWorldDir.z * nextDistance;
			fixed16_t deltaX = fixed16_t::FromRaw((nextIntersectionX - lastIntersectionX).rawValue >> 2);
			fixed16_t deltaZ = fixed16_t::FromRaw((nextIntersectionZ - lastIntersectionZ).rawValue >> 2);

			ditherWriter.Write(ReadTextureFromWorldSpace<64>(mapTexture, lastIntersectionX, lastIntersectionZ));
			lastIntersectionX += deltaX;	lastIntersectionZ += deltaZ;
			ditherWriter.Write(ReadTextureFromWorldSpace<64>(mapTexture, lastIntersectionX, lastIntersectionZ));
			lastIntersectionX += deltaX;	lastIntersectionZ += deltaZ;
			ditherWriter.Write(ReadTextureFromWorldSpace<64>(mapTexture, lastIntersectionX, lastIntersectionZ));
			lastIntersectionX += deltaX;	lastIntersectionZ += deltaZ;
			ditherWriter.Write(ReadTextureFromWorldSpace<64>(mapTexture, lastIntersectionX, lastIntersectionZ));

			lastIntersectionX = nextIntersectionX;
			lastIntersectionZ = nextIntersectionZ;
			lastWorldDir = nextWorldDir;
		}
		else
		{
			ditherWriter.Write(ReadTextureFromWorldSpace<64>(mapTexture, lastIntersectionX, lastIntersectionZ));
			//ditherWriter.Write(0);

			outY += 2;
			steps--;
			if (steps > 0)
			{
				lastWorldDir = Lerp(viewDirTop, viewDirBottom, fixed8_t::FromRaw(outY));
				lastDistance = FixedMath::QuickDivide(fixed16_t(camera.position.y), -lastWorldDir.y);
				lastIntersectionX = camera.position.x + lastWorldDir.x * lastDistance;
				lastIntersectionZ = camera.position.z + lastWorldDir.z * lastDistance;
			}
		}
	}
}

inline void DrawCloudSpan(DitherWriter& ditherWriter, const Vector3b& viewDirTop, const Vector3b& viewDirBottom, uint8_t& outY, uint8_t height)
{
	constexpr fixed16_t cloudHeight = 128;
	fixed16_t planeDistance = cloudHeight - camera.position.y;
	uint8_t steps = height >> 1;

	if (steps >= 4)
	{
		Vector3b lastWorldDir = Lerp(viewDirTop, viewDirBottom, fixed8_t::FromRaw(outY));
		fixed16_t lastDistance = FixedMath::QuickDivide(planeDistance, lastWorldDir.y);
		fixed16_t lastIntersectionX = camera.position.x + lastWorldDir.x * lastDistance;
		fixed16_t lastIntersectionZ = camera.position.z + lastWorldDir.z * lastDistance;

		while (steps >= 4)
		{
			outY += 8;
			steps -= 4;

			Vector3b nextWorldDir = Lerp(viewDirTop, viewDirBottom, fixed8_t::FromRaw(outY));
			fixed16_t nextDistance = FixedMath::QuickDivide(planeDistance, nextWorldDir.y);
			fixed16_t nextIntersectionX = camera.position.x + nextWorldDir.x * nextDistance;
			fixed16_t nextIntersectionZ = camera.position.z + nextWorldDir.z * nextDistance;
			fixed16_t deltaX = fixed16_t::FromRaw((nextIntersectionX - lastIntersectionX).rawValue >> 2);
			fixed16_t deltaZ = fixed16_t::FromRaw((nextIntersectionZ - lastIntersectionZ).rawValue >> 2);

			ditherWriter.Write(ReadTextureFromWorldSpace<32>(cloudTexture, lastIntersectionX, lastIntersectionZ));
			lastIntersectionX += deltaX;	lastIntersectionZ += deltaZ;
			ditherWriter.Write(ReadTextureFromWorldSpace<32>(cloudTexture, lastIntersectionX, lastIntersectionZ));
			lastIntersectionX += deltaX;	lastIntersectionZ += deltaZ;
			ditherWriter.Write(ReadTextureFromWorldSpace<32>(cloudTexture, lastIntersectionX, lastIntersectionZ));
			lastIntersectionX += deltaX;	lastIntersectionZ += deltaZ;
			ditherWriter.Write(ReadTextureFromWorldSpace<32>(cloudTexture, lastIntersectionX, lastIntersectionZ));

			lastIntersectionX = nextIntersectionX;
			lastIntersectionZ = nextIntersectionZ;
		}
	}
	while (steps > 0)
	{
		Vector3b worldDir = Lerp(viewDirTop, viewDirBottom, fixed8_t::FromRaw(outY));
		fixed16_t distance = FixedMath::QuickDivide(planeDistance, worldDir.y);
		fixed16_t intersectionX = camera.position.x + worldDir.x * distance;
		fixed16_t intersectionZ = camera.position.z + worldDir.z * distance;
		ditherWriter.Write(ReadTextureFromWorldSpace<32>(cloudTexture, intersectionX, intersectionZ));

		outY += 2;
		steps--;
	}
}


void GameDrawInterp()
{
	constexpr fixed16_t groundHeight = 0;
	constexpr fixed16_t cloudHeight = 128;
	constexpr int maxDrawDistance = 2000;
	constexpr int skyColour = 3;
	constexpr int groundColour = 2;
	constexpr fixed8_t half = fixed8_t::FromRaw(1 << (FixedFractionalBits - 1));

	constexpr fixed8_t groundThreshold = fixed8_t::FromRaw(-3);		// below this is ground texture
	constexpr fixed8_t cloudThreshold = fixed8_t::FromRaw(8);		// above this is cloud texture

	// Rotate view a bit because bottom area is obscured
	angle_t cameraTilt = 4;
	Matrix3x3 cameraRotation = Matrix3x3::RotateX(cameraTilt) * camera.rotation;

	// Calculate world view direction for each corner of the viewport
	Vector3b topLeftViewDir = cameraRotation * Vector3b(-1, half, 1);
	Vector3b topRightViewDir = cameraRotation * Vector3b(1, half, 1);
	Vector3b bottomLeftViewDir = cameraRotation * Vector3b(-1, -half, 1);
	Vector3b bottomRightViewDir = cameraRotation * Vector3b(1, -half, 1);

	for (uint8_t x = 0; x < DISPLAY_WIDTH; x += 2)
	{
		// Interpolate view directions for this column 
		fixed8_t viewXAlpha = fixed8_t::FromRaw(x >> 1);
		Vector3b viewDirTop = Lerp(topLeftViewDir, topRightViewDir, viewXAlpha);
		Vector3b viewDirBottom = Lerp(bottomLeftViewDir, bottomRightViewDir, viewXAlpha);

		// Left and right thirds are obscured by MFD so render less
		uint8_t displayHeight = x < 40 || x >= DISPLAY_WIDTH - 40 ? DISPLAY_HEIGHT - 24 : DISPLAY_HEIGHT - 16;
		DitherWriter ditherWriter(&Platform::GetScreenBuffer()[x], ditherPattern1, displayHeight >> 1);

		uint8_t outY = 0;

		if (viewDirTop.y > viewDirBottom.y)
		{
			// Right side up
			fixed8_t viewDiff = (viewDirTop.y - viewDirBottom.y);

			if (viewDirTop.y > cloudThreshold)
			{
				// Draw cloud span
				int16_t limit = displayHeight;
				
				if (viewDirBottom.y < cloudThreshold)
				{
					int16_t cloudY = FixedMath::QuickDivide((viewDirTop.y - cloudThreshold), viewDiff).rawValue;
					if (cloudY < limit)
						limit = cloudY;
				}

				DrawCloudSpan(ditherWriter, viewDirTop, viewDirBottom, outY, limit - outY);
			}
			if (ditherWriter.CanWrite() && viewDirTop.y > 0)
			{
				// Draw distant sky span
				int16_t limit = displayHeight;

				if (viewDirBottom.y < 0)
				{
					int16_t horizon = FixedMath::QuickDivide(viewDirTop.y - 0, viewDiff).rawValue;
					if (horizon < limit)
						limit = horizon;
				}

				while (outY < limit) // && ditherWriter.CanWrite())
				{
					ditherWriter.Write(skyColour);
					outY += 2;
				}
			}
			if (ditherWriter.CanWrite() && viewDirTop.y > groundThreshold)
			{
				// Draw distance ground span
				int16_t limit = displayHeight;

				if (viewDirBottom.y < 0)
				{
					int16_t groundY = FixedMath::QuickDivide((viewDirTop.y - groundThreshold), viewDiff).rawValue;
					if (groundY < limit)
						limit = groundY;
				}

				while (outY < limit)
				{
					ditherWriter.Write(groundColour);
					outY += 2;
				}
			}
			if (ditherWriter.CanWrite())
			{
				// Ground plane
				DrawGroundSpan(ditherWriter, viewDirTop, viewDirBottom, outY, displayHeight - outY);
			}
		}
		else if (viewDirTop.y < viewDirBottom.y)
		{
			// Upside down
			fixed8_t viewDiff = (viewDirBottom.y - viewDirTop.y);

			if (viewDirTop.y < groundThreshold)
			{
				// Draw ground span
				int16_t limit = displayHeight;

				if (viewDirBottom.y > groundThreshold)
				{
					int16_t groundY = FixedMath::QuickDivide((groundThreshold - viewDirTop.y), viewDiff).rawValue;
					if (groundY < limit)
						limit = groundY;
				}

				DrawGroundSpan(ditherWriter, viewDirTop, viewDirBottom, outY, limit - outY);
			}
			if (ditherWriter.CanWrite() && viewDirTop.y < 0)
			{
				// Draw distance ground span
				int16_t limit = displayHeight;

				if (viewDirBottom.y > 0)
				{
					int16_t horizon = FixedMath::QuickDivide((0 - viewDirTop.y), viewDiff).rawValue;
					if (horizon < limit)
						limit = horizon;
				}

				while (outY < limit) 
				{
					ditherWriter.Write(groundColour);
					outY += 2;
				}
			}
			if (ditherWriter.CanWrite() && viewDirTop.y < cloudThreshold)
			{
				// Draw distant sky span
				int16_t limit = displayHeight;

				if (viewDirBottom.y > cloudThreshold)
				{
					int16_t cloudY = FixedMath::QuickDivide((cloudThreshold - viewDirTop.y), viewDiff).rawValue;
					if (cloudY < limit)
						limit = cloudY;
				}

				while (outY < limit) 
				{
					ditherWriter.Write(skyColour);
					outY += 2;
				}
			}
			if (ditherWriter.CanWrite())
			{
				// Cloud plane
				DrawCloudSpan(ditherWriter, viewDirTop, viewDirBottom, outY, displayHeight - outY);
			}
		}
		else 
		{
			// 90 degrees sideways
			if (viewDirTop.y > cloudThreshold)
			{
				DrawCloudSpan(ditherWriter, viewDirTop, viewDirBottom, outY, displayHeight);
			}
			else if (viewDirTop.y > 0)
			{
				while (outY < displayHeight)
				{
					ditherWriter.Write(skyColour);
					outY += 2;
				}
			}
			else if (viewDirTop.y > groundThreshold)
			{
				while (outY < displayHeight)
				{
					ditherWriter.Write(groundColour);
					outY += 2;
				}
			}
			else
			{
				DrawGroundSpan(ditherWriter, viewDirTop, viewDirBottom, outY, displayHeight);
			}
		}
	}
}


void GameDrawSimple()
{
	constexpr fixed16_t groundHeight = 0;
	constexpr fixed16_t cloudHeight = 128;
	constexpr int maxDrawDistance = 2000;
	constexpr int skyColour = 3;
	constexpr int groundColour = 2;
	constexpr fixed8_t half = fixed8_t::FromRaw(1 << (FixedFractionalBits - 1));

	camera.roll = 0;
	//camera.pitch = 0;

	if (Platform::GetInput() & INPUT_LEFT)
	{
		camera.yaw--;
	}
	if (Platform::GetInput() & INPUT_RIGHT)
	{
		camera.yaw++;
	}

	Vector3b forward = camera.rotation.Forward();
	Vector3b right = camera.rotation.Right();
	Vector3s intersectionBase = camera.position;
	intersectionBase.y = 0;

	uint8_t* buffer = Platform::GetScreenBuffer();

	for (uint8_t y = 0; y < DISPLAY_HEIGHT; y++)
	{
		const uint8_t* ditherPattern = (y & 1) == 0 ? ditherPattern1 : ditherPattern1 + 2;
		uint8_t writePos = y & 7;
		uint8_t writeMask = 1 << writePos;
		uint8_t clearMask = ~writeMask;

		//fixed8_t worldDirY = FixedMath::Sin(camera.pitch + 32 - y); //fixed8_t::FromRaw(32 - y);
		Vector3b worldDir = camera.rotation * Vector3b(0, fixed8_t::FromRaw(32 - y), 1);
		fixed8_t worldDirY = worldDir.y;

		fixed16_t distance = 0;
		if (worldDirY < fixed8_t::FromRaw(-4))
		{
			// Vector hits the ground plane
			distance = FixedMath::QuickDivide(fixed16_t(camera.position.y), -worldDirY);
		}
		else if (worldDirY > fixed8_t::FromRaw(4))
		{
			distance = FixedMath::QuickDivide(fixed16_t(cloudHeight - camera.position.y), worldDirY);
		}

		fixed16_t intersectionMiddleX = camera.position.x + (forward.x * distance);
		fixed16_t intersectionMiddleZ = camera.position.z + (forward.z * distance);

		fixed16_t rightXDistance = right.x * distance;
		fixed16_t rightZDistance = right.z * distance;

		fixed16_t intersectionX = intersectionMiddleX - rightXDistance;
		fixed16_t intersectionZ = intersectionMiddleZ - rightZDistance;

		fixed16_t deltaX = fixed8_t::FromRaw(1) * rightXDistance;
		fixed16_t deltaZ = fixed8_t::FromRaw(1) * rightZDistance;

		for (uint8_t x = 0; x < DISPLAY_WIDTH; x++)
		{
			//fixed8_t worldDirX = fixed8_t::FromRaw(64 - x);
			//fixed16_t intersectionX = intersectionMiddleX + worldDirX * rightXDistance;
			//fixed16_t intersectionZ = intersectionMiddleZ + worldDirX * rightZDistance;
			intersectionX += deltaX;
			intersectionZ += deltaZ;

			uint8_t u = (int(intersectionX) >> 4);
			uint8_t v = (int(intersectionZ) >> 4);
			uint8_t colour;

			if (worldDirY < 0)
			{
				u &= 63;
				v &= 63;
				colour = pgm_read_byte(&mapTexture[v * 64 + u]);
			}
			else
			{
				u &= 31;
				v &= 31;
				colour = pgm_read_byte(&cloudTexture[v * 32 + u]);
			}

			uint8_t threshold = ditherPattern[x & 1];
			if (colour >= threshold)
			{
				buffer[x] |= writeMask;
			}
			else
			{
				buffer[x] &= clearMask;
			}
		}

		if (writePos == 7)
		{
			buffer += DISPLAY_WIDTH;
		}
	}
}

void GameDrawHalfRes()
{
	constexpr fixed16_t groundHeight = 0;
	constexpr fixed16_t cloudHeight = 128;
	constexpr int maxDrawDistance = 2000;
	constexpr int skyColour = 3;
	constexpr int groundColour = 2;
	constexpr fixed8_t half = fixed8_t::FromRaw(1 << (FixedFractionalBits - 1));

	// Calculate world view direction for each corner of the viewport
	Vector3b topLeftViewDir = camera.rotation * Vector3b(-1, half, 1);
	Vector3b topRightViewDir = camera.rotation * Vector3b(1, half, 1);
	Vector3b bottomLeftViewDir = camera.rotation * Vector3b(-1, -half, 1);
	Vector3b bottomRightViewDir = camera.rotation * Vector3b(1, -half, 1);

	for (uint8_t x = 0; x < DISPLAY_WIDTH; x += 2)
	{
		const uint8_t* ditherPattern = ditherPattern1;
		uint8_t* bufferPtr = &Platform::GetScreenBuffer()[x];
		uint8_t buffer1 = 0, buffer2 = 0;
		uint8_t bufferPos = 0;
		uint8_t writeMask = 1;

		// Interpolate view directions for this column 
		fixed8_t viewXAlpha = fixed8_t::FromRaw(x >> 1);
		Vector3b viewDirTop = Lerp(topLeftViewDir, topRightViewDir, viewXAlpha);
		Vector3b viewDirBottom = Lerp(bottomLeftViewDir, bottomRightViewDir, viewXAlpha);

		// Left and right thirds are obscured by MFD so render less
		uint8_t displayHeight = x < 40 || x >= DISPLAY_WIDTH - 40 ? DISPLAY_HEIGHT - 24 : DISPLAY_HEIGHT - 16;

		for (uint8_t y = 0; y < displayHeight; y ++)
		{
			uint8_t outColour = skyColour;

			Vector3b worldDir = Lerp(viewDirTop, viewDirBottom, fixed8_t::FromRaw(y + 8));

			if (worldDir.y < fixed8_t::FromRaw(-4))
			{
				// Vector hits the ground plane
				fixed16_t distance = FixedMath::QuickDivide(fixed16_t(camera.position.y), -worldDir.y);

				fixed16_t intersectionX = camera.position.x + worldDir.x * distance;
				fixed16_t intersectionZ = camera.position.z + worldDir.z * distance;

				// Convert to texture space 64x64
				uint8_t u = ((int)(intersectionX) >> 4) & 63;
				uint8_t v = ((int)(intersectionZ) >> 4) & 63;

				int index = v * 64 + u;
				outColour = pgm_read_byte(&mapTexture[index]);
			}
			else if (worldDir.y < 0)
			{
				// Hit the ground plane but near the horizon so just assume ground colour to avoid overflow problems
				outColour = groundColour;
			}
			if (worldDir.y > fixed8_t::FromRaw(4))
			{
				// Vector has hit the cloud plane
				fixed16_t distance = FixedMath::QuickDivide(fixed16_t(cloudHeight - camera.position.y), worldDir.y);
				fixed16_t intersectionX = camera.position.x + worldDir.x * distance;
				fixed16_t intersectionZ = camera.position.z + worldDir.z * distance;

				// Convert to texture space 32x32
				uint8_t u = ((int)(intersectionX) >> 4) & 31;
				uint8_t v = ((int)(intersectionZ) >> 4) & 31;

				int index = v * 32 + u;
				outColour = pgm_read_byte(&cloudTexture[index]);
			}

			uint8_t baseDitherIndex = (y & 1) << 1;

			// Fill in 2x1 pixels based on dither pattern
			if (outColour >= ditherPattern[baseDitherIndex])
			{
				buffer1 |= writeMask;
			}
			if (outColour >= ditherPattern[baseDitherIndex + 1])
			{
				buffer2 |= writeMask;
			}
			writeMask <<= 1;

			bufferPos++;
			if (bufferPos == 8)
			{
				// Write out buffer and move on to the next 8 pixels
				bufferPtr[0] = buffer1;
				bufferPtr[1] = buffer2;
				bufferPtr += DISPLAY_WIDTH;
				writeMask = 1;
				bufferPos = 0;
				buffer1 = 0;
				buffer2 = 0;
			}
		}
	}
}


void GameDrawQuarterRes()
{
	constexpr fixed16_t groundHeight = 0;
	constexpr fixed16_t cloudHeight = 128;
	constexpr int maxDrawDistance = 2000;
	constexpr int skyColour = 3;
	constexpr int groundColour = 2;
	constexpr fixed8_t half = fixed8_t::FromRaw(1 << (FixedFractionalBits - 1));
	constexpr fixed8_t groundThreshold = fixed8_t::FromRaw(-3);		// below this is ground texture
	constexpr fixed8_t cloudThreshold = fixed8_t::FromRaw(8);		// above this is cloud texture

	// Calculate world view direction for each corner of the viewport
	Vector3b topLeftViewDir = camera.rotation * Vector3b(-1, half, 1);
	Vector3b topRightViewDir = camera.rotation * Vector3b(1, half, 1);
	Vector3b bottomLeftViewDir = camera.rotation * Vector3b(-1, -half, 1);
	Vector3b bottomRightViewDir = camera.rotation * Vector3b(1, -half, 1);

	for(uint8_t x = 0; x < DISPLAY_WIDTH; x += 2)
	{
		const uint8_t* ditherPattern = ditherPattern1;
		uint8_t* bufferPtr = &Platform::GetScreenBuffer()[x];
		uint8_t buffer1 = 0, buffer2 = 0;
		uint8_t bufferPos = 0;
		uint8_t writeMask = 1;

		// Interpolate view directions for this column 
		fixed8_t viewXAlpha = fixed8_t::FromRaw(x >> 1);
		Vector3b viewDirTop = Lerp(topLeftViewDir, topRightViewDir, viewXAlpha);
		Vector3b viewDirBottom = Lerp(bottomLeftViewDir, bottomRightViewDir, viewXAlpha);

		// Left and right thirds are obscured by MFD so render less
		uint8_t displayHeight = x < 40 || x >= DISPLAY_WIDTH - 40 ? DISPLAY_HEIGHT - 24 : DISPLAY_HEIGHT - 16;

		for(uint8_t y = 0; y < displayHeight; y += 2)
		{
			uint8_t outColour = skyColour;

			Vector3b worldDir = Lerp(viewDirTop, viewDirBottom, fixed8_t::FromRaw(y + 8));

			if(worldDir.y < groundThreshold)
			{
				// Vector hits the ground plane
				fixed16_t distance = FixedMath::QuickDivide(fixed16_t(camera.position.y), -worldDir.y);

				fixed16_t intersectionX = camera.position.x + worldDir.x * distance;
				fixed16_t intersectionZ = camera.position.z + worldDir.z * distance;

				// Convert to texture space 64x64
				uint8_t u = ((int)(intersectionX) >> 4) & 63;
				uint8_t v = ((int)(intersectionZ) >> 4) & 63;

				int index = v * 64 + u;
				outColour = pgm_read_byte(&mapTexture[index]);
			}
			else if (worldDir.y < 0)
			{
				// Hit the ground plane but near the horizon so just assume ground colour to avoid overflow problems
				outColour = groundColour;
			}
			if (worldDir.y > cloudThreshold)
			{
				// Vector has hit the cloud plane
				fixed16_t distance = FixedMath::QuickDivide(fixed16_t(cloudHeight - camera.position.y), worldDir.y);
				fixed16_t intersectionX = camera.position.x + worldDir.x * distance;
				fixed16_t intersectionZ = camera.position.z + worldDir.z * distance;

				// Convert to texture space 32x32
				uint8_t u = ((int)(intersectionX) >> 4) & 31;
				uint8_t v = ((int)(intersectionZ) >> 4) & 31;

				int index = v * 32 + u;
				outColour = pgm_read_byte(&cloudTexture[index]);
			}
			
			// Fill in 2x2 pixels based on dither pattern
			if(outColour >= ditherPattern[0])
			{
				buffer1 |= writeMask;
			}
			if(outColour >= ditherPattern[1])
			{
				buffer2 |= writeMask;
			}
			writeMask <<= 1;
			if(outColour >= ditherPattern[2])
			{
				buffer1 |= writeMask;
			}
			if(outColour >= ditherPattern[3])
			{
				buffer2 |= writeMask;
			}
			writeMask <<= 1;
			
			bufferPos++;
			if(bufferPos == 4)
			{
				// Write out buffer and move on to the next 8 pixels
				bufferPtr[0] = buffer1;
				bufferPtr[1] = buffer2;
				bufferPtr += DISPLAY_WIDTH;
				writeMask = 1;
				bufferPos = 0;
				buffer1 = 0;
				buffer2 = 0;
			}
		}
	}

	/*
	// Draw minimap
	int startX = (int(camera.position.x) >> 4) - 18;
	int startY = (int(camera.position.z) >> 4) - 10;
	for (int x = 0; x < 36; x++)
	{
		for (int y = 0; y < 20; y++)
		{
			uint8_t u = (startX + x) & 63;
			uint8_t v = (startY + y) & 63;
			uint8_t mapValue = pgm_read_byte(&mapTexture[v * 64 + u]);
			Platform::PutPixel(x + 2, DISPLAY_HEIGHT - 3 - y, mapValue > 1 ? COLOUR_WHITE : COLOUR_BLACK);
		}
	}
	int headingX = (int)(FixedMath::Sin(yaw) * fixed16_t(4));
	int headingY = (int)(FixedMath::Cos(yaw) * fixed16_t(4));
	Platform::DrawLine(20, DISPLAY_HEIGHT - 12, 20 - headingX, DISPLAY_HEIGHT - 12 - headingY, COLOUR_BLACK);
	*/
}

// 128x64 resolution rendering - too slow!
void GameDrawFullRes()
{
	constexpr fixed16_t groundHeight = 0;
	constexpr fixed16_t cloudHeight = 128;
	constexpr int maxDrawDistance = 2000;
	constexpr int skyColour = 3;
	constexpr int groundColour = 2;

	Vector3b topLeftViewDir = camera.rotation * Vector3b(-1, fixed8_t::FromRaw(32), 1);
	Vector3b topRightViewDir = camera.rotation * Vector3b(1, fixed8_t::FromRaw(32), 1);
	Vector3b bottomLeftViewDir = camera.rotation * Vector3b(-1, fixed8_t::FromRaw(-32), 1);
	Vector3b bottomRightViewDir = camera.rotation * Vector3b(1, fixed8_t::FromRaw(-32), 1);

	for (uint8_t x = 0; x < DISPLAY_WIDTH; x++)
	{
		const uint8_t* ditherPattern = (x & 1) != 0 ? ditherPattern1 : ditherPattern1 + 2;
		uint8_t* bufferPtr = &Platform::GetScreenBuffer()[x];
		uint8_t buffer = 0;
		uint8_t bufferPos = 0;
		uint8_t writeMask = 1;

		fixed8_t viewXAlpha = fixed8_t::FromRaw(x >> 1);
		Vector3b viewDirTop = Lerp(topLeftViewDir, topRightViewDir, viewXAlpha);
		Vector3b viewDirBottom = Lerp(bottomLeftViewDir, bottomRightViewDir, viewXAlpha);

		uint8_t displayHeight = x < 40 || x >= DISPLAY_WIDTH - 40 ? DISPLAY_HEIGHT - 24 : DISPLAY_HEIGHT - 16;

		for (uint8_t y = 0; y < displayHeight; y++)
		{
			uint8_t outColour = skyColour;

			Vector3b worldDir = Lerp(viewDirTop, viewDirBottom, fixed8_t::FromRaw(y + 8));

			if (worldDir.y < fixed8_t::FromRaw(-4))
			{
				fixed16_t distance = FixedMath::QuickDivide(fixed16_t(camera.position.y), -worldDir.y);
				fixed16_t intersectionX = camera.position.x + worldDir.x * distance;
				fixed16_t intersectionZ = camera.position.z + worldDir.z * distance;

				uint8_t u = ((int)(intersectionX) >> 4) & 63;
				uint8_t v = ((int)(intersectionZ) >> 4) & 63;

				int index = v * 64 + u;
				outColour = pgm_read_byte(&mapTexture[index]);
			}
			else if (worldDir.y < 0)
			{
				outColour = groundColour;
			}
			if (worldDir.y > fixed8_t::FromRaw(4))
			{
				fixed16_t distance = FixedMath::QuickDivide(fixed16_t(cloudHeight - camera.position.y), worldDir.y);
				fixed16_t intersectionX = camera.position.x + worldDir.x * distance;
				fixed16_t intersectionZ = camera.position.z + worldDir.z * distance;

				uint8_t u = ((int)(intersectionX) >> 4) & 31;
				uint8_t v = ((int)(intersectionZ) >> 4) & 31;

				int index = v * 32 + u;
				outColour = pgm_read_byte(&cloudTexture[index]);
			}

			if (outColour >= ditherPattern[y & 1])
			{
				buffer |= writeMask;
			}
			writeMask <<= 1;

			bufferPos++;
			if (bufferPos == 8)
			{
				*bufferPtr = buffer;
				bufferPtr += DISPLAY_WIDTH;
				writeMask = 1;
				bufferPos = 0;
				buffer = 0;
			}
		}
	}
}

void GameDrawHUD()
{
	uint8_t* bufferPtr = &Platform::GetScreenBuffer()[40];
	(*bufferPtr) = 1;
	bufferPtr += DISPLAY_WIDTH;
	for (int y = 1; y < DISPLAY_HEIGHT / 8; y++)
	{
		*bufferPtr = 0;
		bufferPtr += DISPLAY_WIDTH;
	}
	bufferPtr = &Platform::GetScreenBuffer()[DISPLAY_WIDTH - 40];
	(*bufferPtr) = 1;
	bufferPtr += DISPLAY_WIDTH;
	for (int y = 1; y < DISPLAY_HEIGHT / 8; y++)
	{
		*bufferPtr = 0;
		bufferPtr += DISPLAY_WIDTH;
	}

	for (int x = 41; x < DISPLAY_WIDTH - 40; x++)
	{
		Platform::GetScreenBuffer()[x] &= 0xfe;
	}
}

void Game::Draw()
{
	static int drawMethod = 3;

	switch (drawMethod)
	{
	case 0:
		GameDrawQuarterRes();
		Platform::GetScreenBuffer()[0] = 0x80;
		break;
	case 1:
		GameDrawHalfRes();
		Platform::GetScreenBuffer()[0] = 0xc;
		break;
	case 2:
		GameDrawFullRes();
		Platform::GetScreenBuffer()[0] = 0x7f;
		break;
	case 3:
		GameDrawInterp();
		break;
	case 4:
		GameDrawSimple();
		break;
	}

	GameDrawHUD();

	static uint8_t oldButtons = Platform::GetInput();
	if (Platform::GetInput() != oldButtons)
	{
		if (Platform::GetInput() & INPUT_B)
		{
			drawMethod = (drawMethod + 1) % 4;
		}
	}
	
	oldButtons = Platform::GetInput();
}

void Game::Tick()
{
	if (Platform::GetInput() & INPUT_DOWN)
	{
		camera.pitch++;
	}
	if (Platform::GetInput() & INPUT_UP)
	{
		camera.pitch--;
	}
	if (Platform::GetInput() & INPUT_LEFT)
	{
		camera.roll++;
	}
	if (Platform::GetInput() & INPUT_RIGHT)
	{
		camera.roll--;
	}

	int8_t rollSigned = (int8_t)(camera.roll);
	int8_t turn = rollSigned / 8;

	if (turn > 3)
		turn = 3;
	if (turn < -3)
		turn = -3;

	camera.yaw += turn;

	camera.rotation = Matrix3x3::RotateZ(camera.roll) * Matrix3x3::RotateX(-camera.pitch) * Matrix3x3::RotateY(-camera.yaw);
	camera.invRotation = camera.rotation.Transpose();
	camera.position = camera.position + Vector3s(camera.rotation.Forward());

	if (camera.position.y < 1)
		camera.position.y = 1;
	if (camera.position.y > 120)
		camera.position.y = 120;
}


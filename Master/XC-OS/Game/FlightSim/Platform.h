#pragma once

#include <stdint.h>

namespace FlightSim{

class Platform
{
public:
	static uint8_t GetInput(void);
	static void SetLED(uint8_t r, uint8_t g, uint8_t b);
	static uint8_t* GetScreenBuffer(); 

	static void PlaySound(const uint16_t* audioPattern);
	static bool IsAudioEnabled();
	static void SetAudioEnabled(bool isEnabled);

	static void ExpectLoadDelay();
	
	static void FillScreen(uint8_t col);
	static void PutPixel(uint8_t x, uint8_t y, uint8_t colour);
	static void DrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap);
	static void DrawSolidBitmap(int16_t x, int16_t y, const uint8_t *bitmap);
	static void DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap, const uint8_t *mask, uint8_t frame, uint8_t mask_frame);
	static void DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame);	
	static void FillRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t colour);
	static void FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t* pattern);

	//static void FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t *pattern);

	static void DrawFastVLine(int16_t x, int16_t y, uint8_t w, uint8_t pattern);
	//static void FillVLine(int8_t y0_, int8_t y1_, uint8_t pattern, uint8_t *screenptr);
	static void DrawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color);

	static void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);

	static void DrawBackground();
};

}

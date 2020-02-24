// Compact font taken from
// https://hackaday.io/project/6309-vga-graphics-over-spi-and-serial-vgatonic/log/20759-a-tiny-4x6-pixel-font-that-will-fit-on-almost-any-microcontroller-license-mit

#include <stdint.h>
#include "Defines.h"
#include "Font.h"
#include "Platform.h"
#include "VirtualKeyboard.h"

// Font Definition
static const uint8_t font4x6[96][2] PROGMEM = {
	{ 0x00  ,  0x00 },   /*SPACE*/
	{ 0x49  ,  0x08 },   /*'!'*/
	{ 0xb4  ,  0x00 },   /*'"'*/
	{ 0xbe  ,  0xf6 },   /*'#'*/
	{ 0x7b  ,  0x7a },   /*'$'*/
	{ 0xa5  ,  0x94 },   /*'%'*/
	{ 0x55  ,  0xb8 },   /*'&'*/
	{ 0x48  ,  0x00 },   /*'''*/
	{ 0x29  ,  0x44 },   /*'('*/
	{ 0x44  ,  0x2a },   /*')'*/
	{ 0x15  ,  0xa0 },   /*'*'*/
	{ 0x0b  ,  0x42 },   /*'+'*/
	{ 0x00  ,  0x50 },   /*','*/
	{ 0x03  ,  0x02 },   /*'-'*/
	{ 0x00  ,  0x08 },   /*'.'*/
	{ 0x25  ,  0x90 },   /*'/'*/
	{ 0x76  ,  0xba },   /*'0'*/
	{ 0x59  ,  0x5c },   /*'1'*/
	{ 0xc5  ,  0x9e },   /*'2'*/
	{ 0xc5  ,  0x38 },   /*'3'*/
	{ 0x92  ,  0xe6 },   /*'4'*/
	{ 0xf3  ,  0x3a },   /*'5'*/
	{ 0x73  ,  0xba },   /*'6'*/
	{ 0xe5  ,  0x90 },   /*'7'*/
	{ 0x77  ,  0xba },   /*'8'*/
	{ 0x77  ,  0x3a },   /*'9'*/
	{ 0x08  ,  0x40 },   /*':'*/
	{ 0x08  ,  0x50 },   /*';'*/
	{ 0x2a  ,  0x44 },   /*'<'*/
	{ 0x1c  ,  0xe0 },   /*'='*/
	{ 0x88  ,  0x52 },   /*'>'*/
	{ 0xe5  ,  0x08 },   /*'?'*/
	{ 0x56  ,  0x8e },   /*'@'*/
	{ 0x77  ,  0xb6 },   /*'A'*/
	{ 0x77  ,  0xb8 },   /*'B'*/
	{ 0x72  ,  0x8c },   /*'C'*/
	{ 0xd6  ,  0xba },   /*'D'*/
	{ 0x73  ,  0x9e },   /*'E'*/
	{ 0x73  ,  0x92 },   /*'F'*/
	{ 0x72  ,  0xae },   /*'G'*/
	{ 0xb7  ,  0xb6 },   /*'H'*/
	{ 0xe9  ,  0x5c },   /*'I'*/
	{ 0x64  ,  0xaa },   /*'J'*/
	{ 0xb7  ,  0xb4 },   /*'K'*/
	{ 0x92  ,  0x9c },   /*'L'*/
	{ 0xbe  ,  0xb6 },   /*'M'*/
	{ 0xd6  ,  0xb6 },   /*'N'*/
	{ 0x56  ,  0xaa },   /*'O'*/
	{ 0xd7  ,  0x92 },   /*'P'*/
	{ 0x76  ,  0xee },   /*'Q'*/
	{ 0x77  ,  0xb4 },   /*'R'*/
	{ 0x71  ,  0x38 },   /*'S'*/
	{ 0xe9  ,  0x48 },   /*'T'*/
	{ 0xb6  ,  0xae },   /*'U'*/
	{ 0xb6  ,  0xaa },   /*'V'*/
	{ 0xb6  ,  0xf6 },   /*'W'*/
	{ 0xb5  ,  0xb4 },   /*'X'*/
	{ 0xb5  ,  0x48 },   /*'Y'*/
	{ 0xe5  ,  0x9c },   /*'Z'*/
	{ 0x69  ,  0x4c },   /*'['*/
	{ 0x91  ,  0x24 },   /*'\'*/
	{ 0x64  ,  0x2e },   /*']'*/
	{ 0x54  ,  0x00 },   /*'^'*/
	{ 0x00  ,  0x1c },   /*'_'*/
	{ 0x44  ,  0x00 },   /*'`'*/
	{ 0x0e  ,  0xae },   /*'a'*/
	{ 0x9a  ,  0xba },   /*'b'*/
	{ 0x0e  ,  0x8c },   /*'c'*/
	{ 0x2e  ,  0xae },   /*'d'*/
	{ 0x0e  ,  0xce },   /*'e'*/
	{ 0x56  ,  0xd0 },   /*'f'*/
	{ 0x55  ,  0x3B },   /*'g'*/
	{ 0x93  ,  0xb4 },   /*'h'*/
	{ 0x41  ,  0x44 },   /*'i'*/
	{ 0x41  ,  0x51 },   /*'j'*/
	{ 0x97  ,  0xb4 },   /*'k'*/
	{ 0x49  ,  0x44 },   /*'l'*/
	{ 0x17  ,  0xb6 },   /*'m'*/
	{ 0x1a  ,  0xb6 },   /*'n'*/
	{ 0x0a  ,  0xaa },   /*'o'*/
	{ 0xd6  ,  0xd3 },   /*'p'*/
	{ 0x76  ,  0x67 },   /*'q'*/
	{ 0x17  ,  0x90 },   /*'r'*/
	{ 0x0f  ,  0x38 },   /*'s'*/
	{ 0x9a  ,  0x8c },   /*'t'*/
	{ 0x16  ,  0xae },   /*'u'*/
	{ 0x16  ,  0xba },   /*'v'*/
	{ 0x16  ,  0xf6 },   /*'w'*/
	{ 0x15  ,  0xb4 },   /*'x'*/
	{ 0xb5  ,  0x2b },   /*'y'*/
	{ 0x1c  ,  0x5e },   /*'z'*/
	{ 0x6b  ,  0x4c },   /*'{'*/
	{ 0x49  ,  0x48 },   /*'|'*/
	{ 0xc9  ,  0x5a },   /*'}'*/
	{ 0x54  ,  0x00 },   /*'~'*/
	{ 0x56  ,  0xe2 }    /*''*/
};

int16_t Font::cursorX;
int16_t Font::cursorY;

void Font::DrawChar(char c, uint8_t colour)
{
	const uint8_t index = ((unsigned char)(c)) - 32;
	
	if (index > 96)
		return;

	uint8_t data1 = pgm_read_byte(&font4x6[index][0]);
	uint8_t data2 = pgm_read_byte(&font4x6[index][1]);
	int16_t y = cursorY;
	uint8_t fg = colour;
	uint8_t bg = !colour;
	/*
	if (data2 & 1)	// Descender e.g. j, g
	{
		Platform::DrawPixel(cursorX, y, bg);
		Platform::DrawPixel(cursorX + 1, y, bg);
		Platform::DrawPixel(cursorX + 2, y, bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;

		Platform::DrawPixel(cursorX, y, (data1 & 0x80) ? fg : bg);
		Platform::DrawPixel(cursorX + 1, y, (data1 & 0x40) ? fg : bg);
		Platform::DrawPixel(cursorX + 2, y, (data1 & 0x20) ? fg : bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;

		Platform::DrawPixel(cursorX, y, (data1 & 0x10) ? fg : bg);
		Platform::DrawPixel(cursorX + 1, y, (data1 & 0x8) ? fg : bg);
		Platform::DrawPixel(cursorX + 2, y, (data1 & 0x4) ? fg : bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;

		Platform::DrawPixel(cursorX, y, (data1 & 0x2) ? fg : bg);
		Platform::DrawPixel(cursorX + 1, y, (data1 & 0x1) ? fg : bg);
		Platform::DrawPixel(cursorX + 2, y, (data2 & 0x2) ? fg : bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;

		Platform::DrawPixel(cursorX, y, (data2 & 0x80) ? fg : bg);
		Platform::DrawPixel(cursorX + 1, y, (data2 & 0x40) ? fg : bg);
		Platform::DrawPixel(cursorX + 2, y, (data2 & 0x20) ? fg : bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;

		Platform::DrawPixel(cursorX, y, (data2 & 0x10) ? fg : bg);
		Platform::DrawPixel(cursorX + 1, y, (data2 & 0x8) ? fg : bg);
		Platform::DrawPixel(cursorX + 2, y, (data2 & 0x4) ? fg : bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;
	}
	else
	{
		Platform::DrawPixel(cursorX, y, (data1 & 0x80) ? fg : bg);
		Platform::DrawPixel(cursorX + 1, y, (data1 & 0x40) ? fg : bg);
		Platform::DrawPixel(cursorX + 2, y, (data1 & 0x20) ? fg : bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;

		Platform::DrawPixel(cursorX, y, (data1 & 0x10) ? fg : bg);
		Platform::DrawPixel(cursorX + 1, y, (data1 & 0x8) ? fg : bg);
		Platform::DrawPixel(cursorX + 2, y, (data1 & 0x4) ? fg : bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;

		Platform::DrawPixel(cursorX, y, (data1 & 0x2) ? fg : bg);
		Platform::DrawPixel(cursorX + 1, y, (data1 & 0x1) ? fg : bg);
		Platform::DrawPixel(cursorX + 2, y, (data2 & 0x2) ? fg : bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;

		Platform::DrawPixel(cursorX, y, (data2 & 0x80) ? fg : bg);
		Platform::DrawPixel(cursorX + 1, y, (data2 & 0x40) ? fg : bg);
		Platform::DrawPixel(cursorX + 2, y, (data2 & 0x20) ? fg : bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;

		Platform::DrawPixel(cursorX, y, (data2 & 0x10) ? fg : bg);
		Platform::DrawPixel(cursorX + 1, y, (data2 & 0x8) ? fg : bg);
		Platform::DrawPixel(cursorX + 2, y, (data2 & 0x4) ? fg : bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
		y++;

		Platform::DrawPixel(cursorX, y, bg);
		Platform::DrawPixel(cursorX + 1, y, bg);
		Platform::DrawPixel(cursorX + 2, y, bg);
		Platform::DrawPixel(cursorX + 3, y, bg);
	}
*/
	if (data2 & 1)	// Descender e.g. j, g
	{
		y++;

		if(data1 & 0x80)
			Platform::DrawPixel(cursorX, y, fg);
		if (data1 & 0x40)
			Platform::DrawPixel(cursorX + 1, y, fg);
		if (data1 & 0x20)
			Platform::DrawPixel(cursorX + 2, y, fg);
		y++;

		if (data1 & 0x10)
			Platform::DrawPixel(cursorX, y, fg);
		if (data1 & 0x8)
			Platform::DrawPixel(cursorX + 1, y, fg);
		if (data1 & 0x4)
			Platform::DrawPixel(cursorX + 2, y, fg);
		y++;

		if (data1 & 0x2)
			Platform::DrawPixel(cursorX, y, fg);
		if (data1 & 0x1)
			Platform::DrawPixel(cursorX + 1, y, fg);
		if (data2 & 0x2)
			Platform::DrawPixel(cursorX + 2, y, fg);
		y++;

		if (data2 & 0x80)
			Platform::DrawPixel(cursorX, y, fg);
		if (data2 & 0x40)
			Platform::DrawPixel(cursorX + 1, y, fg);
		if (data2 & 0x20)
			Platform::DrawPixel(cursorX + 2, y, fg);
		y++;

		if (data2 & 0x10)
			Platform::DrawPixel(cursorX, y, fg);
		if (data2 & 0x8)
			Platform::DrawPixel(cursorX + 1, y, fg);
		if (data2 & 0x4)
			Platform::DrawPixel(cursorX + 2, y, fg);
		y++;
	}
	else
	{
		if (data1 & 0x80)
			Platform::DrawPixel(cursorX, y, fg);
		if (data1 & 0x40)
			Platform::DrawPixel(cursorX + 1, y, fg);
		if (data1 & 0x20)
			Platform::DrawPixel(cursorX + 2, y, fg);
		y++;

		if (data1 & 0x10)
			Platform::DrawPixel(cursorX, y, fg);
		if (data1 & 0x8)
			Platform::DrawPixel(cursorX + 1, y, fg);
		if (data1 & 0x4)
			Platform::DrawPixel(cursorX + 2, y, fg);
		y++;

		if (data1 & 0x2)
			Platform::DrawPixel(cursorX, y, fg);
		if (data1 & 0x1)
			Platform::DrawPixel(cursorX + 1, y, fg);
		if (data2 & 0x2)
			Platform::DrawPixel(cursorX + 2, y, fg);
		y++;

		if (data2 & 0x80)
			Platform::DrawPixel(cursorX, y, fg);
		if (data2 & 0x40)
			Platform::DrawPixel(cursorX + 1, y, fg);
		if (data2 & 0x20)
			Platform::DrawPixel(cursorX + 2, y, fg);
		y++;

		if (data2 & 0x10)
			Platform::DrawPixel(cursorX, y, fg);
		if (data2 & 0x8)
			Platform::DrawPixel(cursorX + 1, y, fg);
		if (data2 & 0x4)
			Platform::DrawPixel(cursorX + 2, y, fg);
		y++;
	}

	cursorX += glyphWidth;
}

void Font::DrawString(const xString& str, int16_t x, int16_t y, uint8_t colour)
{
	cursorX = x;
	cursorY = y;

	int i = 0;
	while (char c = str[i++])
	{
		DrawChar(c, colour);
	}
}

void Font::DrawCaret(uint8_t colour)
{
	Platform::FillRect(cursorX, cursorY, glyphWidth / 2, glyphHeight, colour);
	cursorX += glyphWidth;
}

void Font::DrawStringWindowed(const xString& str, int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t colour, uint16_t firstLine, uint16_t cursorLocation, bool isFocused)
{
	cursorX = x;
	cursorY = y;

	int columns = w / glyphWidth;
	int i = str.GetLineStartIndex(firstLine, columns);
	int length = str.Length();

	while (i < length)
	{
		int end = str.GetLineEndIndex(i, columns);

		while (i < end)
		{
			bool isCursor = cursorLocation == i;
			char c = str[i++];

			if (isCursor)
			{
				if (isFocused)
				{
					VirtualKeyboard::SetCursorScreenLocation(cursorX, cursorY);
				}
				Platform::FillRect(cursorX, cursorY, glyphWidth, glyphHeight, colour);
				DrawChar(c, !colour);
			}
			else
			{
				DrawChar(c, colour);
			}
		}

		if (i < length)
		{
			cursorY += glyphHeight + 1;
			cursorX = x;

			if (cursorY + glyphHeight >= y + h)
				return;
		}
	}

	// If ends with a new line then move cursor to next line
	if (str[length - 1] == '\n' || str[length - 1] == '\r')
	{
		cursorY += glyphHeight + 1;
		cursorX = x;
	}
	if (cursorLocation == length)
	{
		if (isFocused)
		{
			VirtualKeyboard::SetCursorScreenLocation(cursorX, cursorY);
		}
		Platform::FillRect(cursorX, cursorY, glyphWidth, glyphHeight, colour);
	}
}

void Font::DrawHexNibble(uint8_t val, uint8_t colour)
{
	val &= 0xf;

	if (val < 0xa)
	{
		DrawChar('0' + val, colour);
	}
	else
	{
		DrawChar('A' + val - 0xa, colour);
	}
}

void Font::DrawHexByte(uint8_t val, int16_t x, int16_t y, uint8_t colour)
{
	cursorX = x;
	cursorY = y;
	DrawHexNibble(val >> 4, colour);
	DrawHexNibble(val, colour);
}

void Font::DrawHexInt(uint16_t val, int16_t x, int16_t y, uint8_t colour)
{
	cursorX = x;
	cursorY = y;
	DrawHexNibble((uint8_t)(val >> 12), colour);
	DrawHexNibble((uint8_t)(val >> 8), colour);
	DrawHexNibble((uint8_t)(val >> 4), colour);
	DrawHexNibble((uint8_t)(val), colour);
}

void Font::DrawInt(int16_t val, int16_t x, int16_t y, uint8_t colour)
{
	cursorX = x;
	cursorY = y;
	DrawInt(val, colour);
}

void Font::DrawChar(char c, int16_t x, int16_t y, uint8_t colour)
{
	cursorX = x;
	cursorY = y;
	DrawChar(c, colour);
}

void Font::DrawInt(int16_t val, uint8_t colour)
{
	constexpr int MAX_DIGITS = 5;

	if (val == 0)
	{
		DrawChar('0', colour);
		return;
	}
	else if (val < 0)
	{
		DrawChar('-', colour);
	}

	char buffer[MAX_DIGITS];
	int bufCount = 0;

	for (int n = 0; n < MAX_DIGITS && val != 0; n++)
	{
		unsigned char c = val % 10;
		buffer[bufCount++] = '0' + c;
		val = val / 10;
	}

	for (int n = bufCount - 1; n >= 0; n--)
	{
		DrawChar(buffer[n], colour);
	}
}


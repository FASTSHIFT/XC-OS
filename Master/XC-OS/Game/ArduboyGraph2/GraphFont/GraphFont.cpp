#include "GraphFont.h"
#include "GraphFontSprite.cpp"

#define GRAPHFONT_HEIGHT 6

#define GRAPHFONT_ATLAS_MIN 33

GraphFont::GraphFont(uint8_t *screenBuffer, int16_t width, int16_t height) {
	sBuffer = screenBuffer;
	sWidth = width;
	sHeight = height;

	// default values
	lineHeight = GRAPHFONT_HEIGHT + 1;
	letterSpacing = 1;

	cursorX = cursorY = baseX = 0;
	textColor = 1;

	maskText = false;
}

size_t GraphFont::write(uint8_t c) {
	if (c == '\r') {
		// this is useless
		// most people use \n for a new line anyway.
	}
	else if (c == '\n') {
		cursorX = baseX; // '\r'
		cursorY += lineHeight; // '\n'
	}
	// check for tab
	else if (c == '\t') {
		cursorX += 8;
	}
	// check for space
	else if (c == ' ') {
		cursorX += 2 + letterSpacing;
	}
	else{
		// draw char
		printChar(c, cursorX, cursorY);
		cursorX += getCharWidth(c) + letterSpacing;
	}
	
	return 1;
}

void GraphFont::printChar(char c, int16_t x, int16_t y)
{
	// no need to draw at all of we're offscreen
	if (x > sWidth - 1 || y + GRAPHFONT_HEIGHT <= 0 || y > sHeight - 1)
		return;
	
	// check if char is available
	if (((uint8_t) c) < 32 || ((uint8_t) c) > 126) c = '?';
	
//	uint8_t cval = ((uint8_t) c) - 33;
	
	// get sprite frames
	const uint8_t *letter = GRAPHFONT_SPRITE + getCharIndex(c);
	
	uint8_t w = getCharWidth(c);
	
	for (uint8_t i = 0; i < w; i++) {
		
		uint8_t colData = pgm_read_byte(letter++);
		
		if (maskText) {
			drawByte(x+i, y, 0xff, (textColor == 0)?1:0);
		}
		
		drawByte(x+i, y, colData, textColor);
	}
}

void GraphFont::drawByte(int16_t x, int16_t y, uint8_t pixels, uint8_t color) {
	// TODO: fix ALL THE BUGS
	
	uint8_t row = (uint8_t)y / 8;

	// check if byte needs to be seperated
	if (((uint8_t)y)%8 == 0) {
		uint8_t col = (uint8_t)x % sWidth;
		if (color == 0)
			sBuffer[col + row*sWidth] &= ~pixels;
		else
			sBuffer[col + row*sWidth] |= pixels;
	}
	else{
		uint8_t d = (uint8_t)y%8;

		drawByte(x, row*8, pixels << d, color);
		drawByte(x, (row+1)*8, pixels >> (8-d), color);
	}
}

uint8_t GraphFont::getCharWidth(uint8_t c) {
	return pgm_read_byte(GRAPHFONT_ATLAS + c - GRAPHFONT_ATLAS_MIN + 1) - pgm_read_byte(GRAPHFONT_ATLAS + c - GRAPHFONT_ATLAS_MIN);
}
uint8_t GraphFont::getCharsWidth(const char* c) {
	uint8_t r = 0;
	uint8_t i = 0;
	
	while (c[i] != '\0') r += getCharWidth(c[i++]);
	r += letterSpacing * (i - 1);
	
	return r;
}
uint8_t GraphFont::getCharsWidth(const __FlashStringHelper** c) {
	uint8_t r = 0;
	uint8_t i = 0;
	
	char d = pgm_read_byte(c + i);
	
	while (d != '\0') {
		r += getCharWidth(d);
		d = pgm_read_byte(c + ++i);
	}
	
	r += letterSpacing * (i - 1);
	
	return r;
}

void GraphFont::setCursor(int16_t x, int16_t y) {
	cursorX = baseX = x;
	cursorY = y;
}

int16_t GraphFont::getCursorX() {
	return cursorX;
}

int16_t GraphFont::getCursorY() {
	return cursorY;
}

void GraphFont::setTextColor(uint8_t color) {
	textColor = color;
}

uint8_t GraphFont::getCharIndex(uint8_t c) {
	return pgm_read_byte(GRAPHFONT_ATLAS + c - GRAPHFONT_ATLAS_MIN);
}

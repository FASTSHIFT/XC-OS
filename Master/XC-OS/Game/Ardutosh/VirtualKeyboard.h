#pragma once

#include <stdint.h>

class VirtualKeyboard
{
public:
	static void Update(bool fullRefresh);
	static bool IsVisible() { return keyboardVisible; }
	static void Show();
	static void SetHexInputLayout();
	static char GetLastKeyPressed() { return lastKeyPressed;  }

	static void SetCursorScreenLocation(int x, int y) { cursorScreenLocationX = x;  cursorScreenLocationY = y; }
	static void ApplyScreenShift(int& x, int&y);
	static void UnapplyScreenShift(int& x, int&y);
private:
	static void DrawSelectionBox(int x, int y, int w, int h);
	static int CountKeysOnRow(const char* c);
	static int GetSpacebarWidth(const char* c);

	static const char* currentLayout;
	static uint8_t selectedKeyX, selectedKeyY;
	static char lastKeyPressed;
	static bool keyboardVisible;
	static int cursorScreenLocationX, cursorScreenLocationY;

	static int screenShiftX, screenShiftY;
};

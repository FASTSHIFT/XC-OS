#include "VirtualKeyboard.h"
#include "Platform.h"
#include "Font.h"
#include "System.h"
#include "Generated/Sprites.h"
// \a = shift
// \v = sym/abc
// \b = backspace
// \r = return

const char keyboardLayout[] PROGMEM =
	"qwertyuiop\n"
	"asdfghjkl\n"
	"\azxcvbnm\b\n"
	"\v,     .\r";

const char keyboardHexLayout[] PROGMEM =
	"789AB\n"
	"456CD\n"
	"123EF\n"
	"0";
const char keyboardLayoutUpper[] PROGMEM =
	"QWERTYUIOP\n"
	"ASDFGHJKL\n"
	"\aZXCVBNM\b\n"
	"\v,     .\r";

const char keyboardLayoutSym[] PROGMEM =
	"1234567890\n"
	"!\"#$%^&*()\n"
	"[];'#-+=_\b\n"
	"\v\\|    /\r";

const char* VirtualKeyboard::currentLayout = keyboardLayout;
uint8_t VirtualKeyboard::selectedKeyX;
uint8_t VirtualKeyboard::selectedKeyY;
bool VirtualKeyboard::keyboardVisible;
char VirtualKeyboard::lastKeyPressed;
int VirtualKeyboard::cursorScreenLocationX;
int VirtualKeyboard::cursorScreenLocationY;
int VirtualKeyboard::screenShiftX;
int VirtualKeyboard::screenShiftY;
static uint8_t lastInput = Platform::GetInput();

int VirtualKeyboard::CountKeysOnRow(const char* c)
{
	int keys = 0;
	while (pgm_read_byte(c))
	{
		if (pgm_read_byte(c) == '\n')
			break;
		keys++;
		c++;
	}
	return keys;
}

int VirtualKeyboard::GetSpacebarWidth(const char* c)
{
	int count = 0;

	while (pgm_read_byte(c) == ' ')
	{
		count++;
		c++;
	}
	return count;
}

void VirtualKeyboard::DrawSelectionBox(int x, int y, int w, int h)
{
	static int animation = 0;
	animation++;
	int movement = (animation & 4) ? 0 : 1;

	for (int n = movement; n < w; n += 2)
	{
		Platform::DrawPixel(x + n, y, BLACK);
		Platform::DrawPixel(x + w - n - 1, y + h - 1, BLACK);
	}
	for(int n = movement; n < h; n += 2)
	{
		Platform::DrawPixel(x + w - 1, y + n, BLACK);
		Platform::DrawPixel(x, y + h - n - 1, BLACK);
	}
}

void VirtualKeyboard::Show()
{
	currentLayout = keyboardLayout;
	keyboardVisible = true;
	lastInput = Platform::GetInput();
	System::MarkScreenDirty();
}

void VirtualKeyboard::SetHexInputLayout()
{
	currentLayout = keyboardHexLayout;
}

void VirtualKeyboard::Update(bool fullRefresh)
{
	uint8_t input = Platform::GetInput();

	if ((input & INPUT_A) && !(lastInput & INPUT_A))
	{
		if (PlatformRemote::IsGamepadEnabled())
		{

		}
		else if (PlatformRemote::IsMouseEnabled())
		{
			PlatformRemote::SetMouseEnabled(false);
			System::MarkScreenDirty();
		}
		else
		{
			// Toggle keyboard
			keyboardVisible = !keyboardVisible;

			if (keyboardVisible)
			{
				Show();
			}
			System::MarkScreenDirty();
			lastInput = input;
			return;
		}
	}

	if (keyboardVisible)
	{
		const int columns = CountKeysOnRow(currentLayout);
		constexpr int rows = 4;
		constexpr int keySpacing = 8;
		constexpr int paddingX = 2;
		constexpr int paddingY = 1;
		int keyboardWidth = columns * keySpacing + paddingX * 2;
		constexpr int keyboardHeight = rows * keySpacing + paddingY * 2;

		int keyboardX = DISPLAY_WIDTH / 2 - keyboardWidth / 2;
		constexpr int keyboardY = DISPLAY_HEIGHT - keyboardHeight + paddingY * 2;

		Platform::FillRect(keyboardX - paddingX, keyboardY - paddingY, keyboardWidth, keyboardHeight, WHITE);
		Platform::DrawRect(keyboardX - paddingX, keyboardY - paddingY, keyboardWidth, keyboardHeight, BLACK);

		int keysOnRow = CountKeysOnRow(currentLayout);
		int outX = keyboardX + (columns - keysOnRow) * keySpacing / 2;
		int outY = keyboardY;
		uint8_t keyX = 0, keyY = 0;

		char selectedChar = 0;
		uint8_t prevSelectedX = selectedKeyX;
		uint8_t prevSelectedY = selectedKeyY;

		if ((input & INPUT_LEFT) && !(lastInput & INPUT_LEFT) && selectedKeyX > 0)
		{
			selectedKeyX--;
		}
		if ((input & INPUT_RIGHT) && !(lastInput & INPUT_RIGHT))
		{
			selectedKeyX++;
		}
		if ((input & INPUT_UP) && !(lastInput & INPUT_UP) && selectedKeyY > 0)
		{
			selectedKeyY--;
		}
		if ((input & INPUT_DOWN) && !(lastInput & INPUT_DOWN) && selectedKeyY < rows - 1)
		{
			selectedKeyY++;
		}

		for (const char* ptr = currentLayout; pgm_read_byte(ptr); ptr++)
		{
			const char c = pgm_read_byte(ptr);
			if (selectedKeyY == keyY && selectedKeyX >= keysOnRow)
			{
				selectedKeyX = keysOnRow - 1;
			}

			if (selectedKeyX == keyX && selectedKeyY == keyY && (input & INPUT_B))
			{
				Platform::FillRect(outX, outY, keySpacing, keySpacing, BLACK);
			}

			switch (c)
			{
			case ' ':
			{
				int spacebarWidth = GetSpacebarWidth(ptr);

				if (selectedKeyY == keyY && selectedKeyX >= keyX && selectedKeyX < keyX + spacebarWidth)
				{
					// A bit hacky
					if (selectedKeyY != prevSelectedY || prevSelectedX < keyX || prevSelectedX >= keyX + spacebarWidth)
					{
						selectedKeyX = keyX + spacebarWidth / 2;
					}
					else if (selectedKeyX < prevSelectedX)
					{
						selectedKeyX = keyX - 1;
					}
					else if (selectedKeyX > prevSelectedX)
					{
						selectedKeyX = keyX + spacebarWidth;
					}

					if (input & INPUT_B)
					{
						Platform::FillRect(outX, outY, spacebarWidth * keySpacing, keySpacing, BLACK);
					}
					else
					{
						DrawSelectionBox(outX, outY, spacebarWidth * keySpacing, keySpacing);
					}
					selectedChar = ' ';
				}
				else
				{
					Platform::DrawRect(outX, outY, spacebarWidth * keySpacing, keySpacing, BLACK);
				}
				ptr += spacebarWidth - 1;
				keyX += spacebarWidth;
				outX += spacebarWidth * keySpacing;
			}
			continue;
			case '\n':
				keysOnRow = CountKeysOnRow(ptr + 1);
				outX = keyboardX + (columns - keysOnRow) * keySpacing / 2;
				outY += keySpacing;
				keyY++;
				keyX = 0;
				continue;
			case '\r':
				Platform::DrawSprite(outX, outY, returnIcon, 0);
				break;
			case '\a':
				Platform::DrawSprite(outX, outY, shiftIcon, 0);
				break;
			case '\b':
				Platform::DrawSprite(outX, outY, backspaceIcon, 0);
				break;
			case '\v':
				if (currentLayout == keyboardLayoutSym)
					Platform::DrawSprite(outX, outY, alphaIcon, 0);
				else
					Platform::DrawSprite(outX, outY, numbersIcon, 0);
				break;
			default:
				//Platform::FillRect(outX, outY, keySpacing, keySpacing, WHITE);
				if (selectedKeyX == keyX && selectedKeyY == keyY && (input & INPUT_B))
				{
					Font::DrawChar(c, outX + paddingX, outY + paddingY, WHITE);
				}
				else
				{
					Font::DrawChar(c, outX + paddingX, outY + paddingY, BLACK);
				}
				break;
			}

			if (selectedKeyX == keyX && selectedKeyY == keyY)
			{
				DrawSelectionBox(outX, outY, keySpacing, keySpacing);
				selectedChar = c;
			}

			outX += keySpacing;
			keyX++;
		}

		if (!(input & INPUT_B) && (lastInput & INPUT_B))
		{
			switch (selectedChar)
			{
			case '\a':
				if (currentLayout == keyboardLayoutUpper)
					currentLayout = keyboardLayout;
				else
					currentLayout = keyboardLayoutUpper;
				break;
			case '\v':
				if (currentLayout == keyboardLayoutSym)
					currentLayout = keyboardLayout;
				else
					currentLayout = keyboardLayoutSym;
				break;
			case '\r':
				lastKeyPressed = '\n';
				System::HandleEvent(SystemEvent::KeyPressed);
				break;
			default:
				lastKeyPressed = selectedChar;
				System::HandleEvent(SystemEvent::KeyPressed);
				break;
			}
		}
	}

	lastInput = input;

	if (fullRefresh)
	{
		int lastScreenShiftX = screenShiftX;
		int lastScreenShiftY = screenShiftY;
		screenShiftX = 0;
		screenShiftY = 0;

		constexpr int obscuredSize = 39;
		constexpr int lastVisibleY = DISPLAY_HEIGHT - obscuredSize;
		constexpr int lastVisibleX = DISPLAY_WIDTH - 8;

		if (cursorScreenLocationY + lastScreenShiftY > lastVisibleY)
		{
			screenShiftY = (cursorScreenLocationY + lastScreenShiftY) - lastVisibleY;
		}
		if (cursorScreenLocationX + lastScreenShiftX < 0)
		{
			screenShiftX = (cursorScreenLocationX + lastScreenShiftX);
		}
		if (cursorScreenLocationX + lastScreenShiftX > lastVisibleX)
		{
			screenShiftX = (cursorScreenLocationX + lastScreenShiftX) - lastVisibleX;
		}

		if (keyboardVisible && (lastScreenShiftY != screenShiftY || lastScreenShiftX != screenShiftX))
		{
			System::MarkScreenDirty();
		}
	}
}

void VirtualKeyboard::ApplyScreenShift(int& x, int&y)
{
	if (keyboardVisible)
	{
		x -= screenShiftX;
		y -= screenShiftY;
	}
}

void VirtualKeyboard::UnapplyScreenShift(int& x, int&y)
{
	if (keyboardVisible)
	{
		x += screenShiftX;
		y += screenShiftY;
	}
}

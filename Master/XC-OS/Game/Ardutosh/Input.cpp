#include "Input.h"
#include "Platform.h"
#include "Defines.h"
#include "Window.h"
#include "System.h"
#include "Generated/Sprites.h"

void VirtualMouse::Tick()
{
	RestoreBackgroundPixels();

	int dirX = 0;
	int dirY = 0;
	uint8_t input = Platform::GetInput();

	if (input & INPUT_LEFT)
	{
		dirX--;
	}
	if (input & INPUT_RIGHT)
	{
		dirX++;
	}
	if (input & INPUT_UP)
	{
		dirY--;
	}
	if (input & INPUT_DOWN)
	{
		dirY++;
	}

	constexpr int mouseAcceleration = 16;
	constexpr int startingAcceleration = 128;

	if (dirX != 0)
	{
		if (velX == 0)
		{
			velX = dirX * startingAcceleration;
		}
		else velX += dirX * mouseAcceleration;
	}
	else velX = 0;

	if (dirY != 0)
	{
		if (velY == 0)
		{
			velY = dirY * startingAcceleration;
		}
		else velY += dirY * mouseAcceleration;
	}
	else velY = 0;

	uint8_t oldX = x;
	uint8_t oldY = y;

	if (velX < 0 && -velX > fixedX)
	{
		fixedX = 0;
	}
	else if (velX > 0 && fixedX + velX >= DISPLAY_WIDTH * 256)
	{
		x = DISPLAY_WIDTH - 1;
		fracX = 0;
	}
	else
	{
		fixedX += velX;
	}

	if (velY < 0 && -velY > fixedY)
	{
		fixedY = 0;
	}
	else if (fixedY + velY >= DISPLAY_HEIGHT * 256)
	{
		y = DISPLAY_HEIGHT - 1;
		fracY = 0;
	}
	else
	{
		fixedY += velY;
	}

	deltaX = x - oldX;
	deltaY = y - oldY;

	/*
	if (draggedWindow < maxWindows)
	{
	Window& targetWindow = windows[draggedWindow];
	if (targetWindow.x + targetWindow.w - windowBarHeight + deltaX < 0)
	{
	deltaX = 0 - (targetWindow.x + targetWindow.w - windowBarHeight);
	}
	if (targetWindow.y + deltaY < menuBarHeight)
	{
	deltaY = menuBarHeight - targetWindow.y;
	}
	if (targetWindow.x + windowBarHeight + deltaX >= DISPLAY_WIDTH)
	{
	deltaX = DISPLAY_WIDTH - 1 - targetWindow.x - windowBarHeight;
	}
	if (targetWindow.y + windowBarHeight + deltaY >= DISPLAY_HEIGHT)
	{
	deltaY = DISPLAY_HEIGHT - 1 - targetWindow.y - windowBarHeight;
	}

	targetWindow.x += deltaX;
	targetWindow.y += deltaY;

	x = oldX + deltaX;
	y = oldY + deltaY;

	}
	*/

	if (PlatformRemote::IsGamepadEnabled())
	{
		lastInput = input;
		x = oldX;
		y = oldY;
	}
	else if (PlatformRemote::IsMouseEnabled())
	{
		if (deltaX != 0 || deltaY != 0)
		{
			PlatformRemote::MouseMove(deltaX * remoteMouseSpeed, deltaY * remoteMouseSpeed);
		}
		if ((input & INPUT_B) && !(lastInput & INPUT_B))
		{
			PlatformRemote::MouseDown();
		}
		if (!(input & INPUT_B) && (lastInput & INPUT_B))
		{
			PlatformRemote::MouseUp();
		}

		lastInput = input;
		x = oldX;
		y = oldY;
	}
	else
	{
		if (deltaX != 0 || deltaY != 0)
		{
			System::HandleEvent(SystemEvent::MouseMove);
		}

		if ((input & INPUT_B) && !(lastInput & INPUT_B))
		{
			System::HandleEvent(SystemEvent::MouseDown);
		}
		if (!(input & INPUT_B) && (lastInput & INPUT_B))
		{
			System::HandleEvent(SystemEvent::MouseUp);
		}

		lastInput = input;
		x = oldX + deltaX;
		y = oldY + deltaY;

		Draw();
	}
}

void VirtualMouse::Draw()
{
	uint8_t* bgPtr = backgroundPixels;

	for (int j = 0; j < 24; j += 8)
	{
		if (y + j >= DISPLAY_HEIGHT)
			break;

		uint8_t* screenPtr = Platform::GetScreenBuffer() + DISPLAY_WIDTH * ((y + j) / 8) + x;

		for (int i = 0; i < cursorWidth; i++)
		{
			if (x + i >= DISPLAY_WIDTH)
				continue;
			*bgPtr++ = *screenPtr++;
		}
	}

	if (PlatformRemote::IsMouseEnabled())
		return;

	Platform::DrawSprite(x, y, mouseCursorSprite, 0);
}

void VirtualMouse::RestoreBackgroundPixels()
{
	uint8_t* bgPtr = backgroundPixels;

	for (int j = 0; j < 24; j += 8)
	{
		if (y + j >= DISPLAY_HEIGHT)
			break;

		uint8_t* screenPtr = Platform::GetScreenBuffer() + DISPLAY_WIDTH * ((y + j) / 8) + x;

		for (int i = 0; i < cursorWidth; i++)
		{
			if (x + i >= DISPLAY_WIDTH)
				continue;
			*screenPtr++ = *bgPtr++;
		}
	}
}

#pragma once

#include "WindowManager.h"
#include "System.h"
#include "Platform.h"
#include "Defines.h"
#include "Input.h"
#include "MenuBar.h"
#include "VirtualKeyboard.h"

uint16_t WindowManager::menuBarItemMask;

void WindowManager::Init()
{
	for (int n = 0; n < maxWindows; n++)
	{
		drawOrder[n] = invalidWindowHandle;
	}
}

void WindowManager::DrawWindow(Window& win)
{
	bool isWindowAnimating = (System::state.currentState == System::State::OpeningWindowAnimation
		|| System::state.currentState == System::State::ClosingWindowAnimation)
		&& System::state.stateElement.window == win.GetHandle();

	if (!isWindowAnimating)
	{
		VirtualKeyboard::ApplyScreenShift(win.x, win.y);
		win.Draw();
		VirtualKeyboard::UnapplyScreenShift(win.x, win.y);
	}
}

void WindowManager::Draw()
{
	for (int n = maxWindows - 1; n >= 0; n--)
	{
		if (drawOrder[n] == invalidWindowHandle)
			continue;
		Window& win = windows[drawOrder[n]];
		DrawWindow(win);
	}
}

void WindowManager::Destroy(Window* window)
{
	int drawPosition = GetDrawPosition(window->GetHandle());
	if (drawPosition != invalidWindowHandle)
	{
		for (int n = drawPosition; n < maxWindows; n++)
		{
			if (n == maxWindows - 1)
			{
				drawOrder[n] = invalidWindowHandle;
			}
			else
			{
				drawOrder[n] = drawOrder[n + 1];
			}
		}
	}

	window->type = WindowType::Closed;
	System::MarkScreenDirty();
}

Window* WindowManager::Create(WindowType type, WindowHandler handler, void* data)
{
	for (int n = 0; n < maxWindows; n++)
	{
		if (windows[n].type == WindowType::Closed)
		{
			Window* newWindow = &windows[n];
			newWindow->type = type;
			newWindow->handler = handler;
			newWindow->x = 0;
			newWindow->y = MenuBar::height;
			newWindow->w = 50;
			newWindow->y = 50;
			newWindow->data = data;

			for (int i = 0; i < maxWindows; i++)
			{
				if (drawOrder[i] == invalidWindowHandle)
				{
					drawOrder[i] = n;
					break;
				}
			}
			Focus(newWindow);

			newWindow->HandleEvent(SystemEvent::OpenWindow);

			return newWindow;
		}
	}
	return nullptr;
}

Window* WindowManager::GetWindow(int16_t x, int16_t y)
{
	for (int n = 0; n < maxWindows; n++)
	{
		if (drawOrder[n] == invalidWindowHandle)
			break;
		Window& win = windows[drawOrder[n]];
		if (x >= win.x && y >= win.y && x < win.x + win.w && y < win.y + win.h)
		{
			return &win;
		}

		if (win.type == WindowType::Desktop)
			return &win;
	}

	return nullptr;
}

Window* WindowManager::GetWindow(WindowHandle handle)
{
	if (handle < maxWindows && windows[handle].type != WindowType::Closed)
		return &windows[handle];
	return nullptr;
}

void WindowManager::DrawAnimations()
{
	if (System::state.currentState == System::State::OpeningWindowAnimation)
	{
		if (System::state.animationTimer >= numAnimationFrames)
		{
			System::ExitState(System::State::OpeningWindowAnimation);
			System::MarkScreenDirty();
		}
		else
		{
			Window* window = GetWindow(System::state.stateElement.window);
			if (window)
			{
				for (int lerp = 1; lerp <= System::state.animationTimer; lerp++)
				{
					int oneMinusLerp = (numAnimationFrames - lerp);

					int x1 = (window->x * lerp + oneMinusLerp * window->originX) / numAnimationFrames;
					int y1 = (window->y * lerp + oneMinusLerp * window->originY) / numAnimationFrames;
					int w1 = (window->w * lerp) / numAnimationFrames;
					int h1 = (window->h * lerp) / numAnimationFrames;

					Platform::DrawRect(x1, y1, w1, h1, BLACK);
				}
			}
			else
			{
				System::ExitState(System::State::OpeningWindowAnimation);
			}
		}
	}
}

bool WindowManager::HandleEvent(SystemEvent eventType)
{
	if (eventType == SystemEvent::Tick)
	{
		for (int n = 0; n < maxWindows; n++)
		{
			if (windows[n].type != WindowType::Closed)
			{
				windows[n].HandleEvent(eventType);
			}
		}

		DrawAnimations();
	}
	else if (eventType == SystemEvent::Repaint)
	{
		Draw();
		DrawAnimations();
	}
	else if (eventType == SystemEvent::MouseDown)
	{
		Window* window = GetWindow(mouse.x, mouse.y);
		if (window)
		{
			if (!ShowingDialog() || window->type == WindowType::DialogBox)
			{
				Focus(window);

				window->HandleEvent(eventType);
				return true;
			}
		}
	}
	else
	{
		if (System::state.currentState != System::State::Default)
		{
			if (System::state.stateElement.window != invalidWindowHandle)
			{
				Window* window = &windows[System::state.stateElement.window];
				window->HandleEvent(eventType);
				return false;
			}
		}

		for (int n = 0; n < maxWindows; n++)
		{
			if (drawOrder[n] != invalidWindowHandle)
			{
				windows[drawOrder[n]].HandleEvent(eventType);
				break;
			}
		}
	}

	if (eventType == SystemEvent::MenuItemClicked)
	{
		switch (MenuBar::GetSelectedMenuItem())
		{
		case Menu_Special_Restart:
			{
				Platform::Reboot();
			}
			break;
		case Menu_Special_CloseAll:
		{
			for (int n = 0; n < maxWindows; n++)
			{
				Window& win = windows[n];
				if (win.type == WindowType::FullWindow)
				{
					Destroy(&win);
				}
			}
		}
			break;
		}
	}

	return false;
}

int WindowManager::GetDrawPosition(WindowHandle handle)
{
	for (int n = 0; n < maxWindows; n++)
	{
		if (drawOrder[n] == invalidWindowHandle)
			break;
		if (drawOrder[n] == handle)
		{
			return n;
		}
	}

	return invalidWindowHandle;
}

void WindowManager::Focus(Window* windowObj)
{
	if (windowObj->type == WindowType::Desktop)
		return;

	WindowHandle window = windowObj->GetHandle();
	int currentPosition = GetDrawPosition(window);

	if (currentPosition == 0)
		return;

	for (int n = currentPosition; n > 0; n--)
	{
		drawOrder[n] = drawOrder[n - 1];
	}

	drawOrder[0] = window;

	System::MarkScreenDirty();
}

Window* WindowManager::FindByData(void* data)
{
	for (uint8_t n = 0; n < maxWindows; n++)
	{
		if (windows[n].type != WindowType::Closed && windows[n].data == data)
		{
			return &windows[n];
		}
	}

	return nullptr;
}

Window* WindowManager::FindByHandler(WindowHandler handler)
{
	for (uint8_t n = 0; n < maxWindows; n++)
	{
		if (windows[n].type != WindowType::Closed && windows[n].handler == handler)
		{
			return &windows[n];
		}
	}

	return nullptr;
}

Window* WindowManager::GetDesktop()
{
	for (uint8_t n = 0; n < maxWindows; n++)
	{
		if (windows[n].type == WindowType::Desktop)
		{
			return &windows[n];
		}
	}

	return nullptr;
}

uint16_t WindowManager::GetMenuBarItemMask()
{
	//System::State oldSystemState = System::state.currentState;
	//System::state.currentState = SystemEvent::GatherMenuItems;
	//menuBarItemMask = 0;
	//
	//for (int n = 0; n < maxWindows; n++)
	//{
	//	if (drawOrder[n] < maxWindows)
	//	{
	//		Window& win = windows[drawOrder[n]];
	//		if (win.type != WindowType::Closed)
	//		{
	//			win.HandleEvent(SystemEvent::GatherMenuItems);
	//			if (menuBarItemMask)
	//				break;
	//		}
	//	}
	//}

	if (menuBarItemMask)
	{
		menuBarItemMask |= Menu_Special_Restart | Menu_Special_CloseAll;
	}

	return menuBarItemMask;
}

bool WindowManager::ShowingDialog()
{
	if (drawOrder[0] < maxWindows)
	{
		return windows[drawOrder[0]].type == WindowType::DialogBox;
	}
	return false;
}

void WindowManager::RepaintFocusedWindow()
{
	if (drawOrder[0] != invalidWindowHandle)
	{
		DrawWindow(windows[drawOrder[0]]);
	}
}

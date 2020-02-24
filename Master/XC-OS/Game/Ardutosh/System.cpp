#include "Defines.h"
#include "System.h"
#include "Platform.h"
#include "Font.h"
#include "Window.h"
#include "Desktop.h"
#include "Input.h"
#include "WindowManager.h"
#include "MenuBar.h"
#include "VirtualKeyboard.h"

#include "Generated/Sprites.inc.h"

VirtualMouse mouse;
bool System::screenDirty = true;
bool System::focusedWindowDirty = false;
System::StateData System::state;

void TestHandler(Window* window)
{
	if (window->Button("Test button", 10, 10))
	{
		WindowManager::Destroy(window);
	}
}

void System::Init()
{
	WindowManager::Init();

	mouse.x = DISPLAY_WIDTH / 2;
	mouse.y = DISPLAY_HEIGHT / 2;

	Desktop::ShowSplash();
	//Desktop::CreateDesktop();

/*	Window* win = WindowManager::Create(WindowType::FullWindow);
	win->title = "Test";
	win->x = 1;
	win->y = 9;
	win->w = 93;
	win->h = 52;
	win->handler = TestHandler;

	win = WindowManager::Create(WindowType::FullWindow);
	win->title = "Test";
	win->x = 9;
	win->y = 18;
	win->w = 100;
	win->h = 40;
	win->handler = TestHandler;
	*/
}


void System::Tick()
{
	HandleEvent(SystemEvent::Tick);

	if (!VirtualKeyboard::IsVisible())
	{
		mouse.Tick();
	}

	state.animationTimer++;
}

void System::MarkScreenDirty()
{
	screenDirty = true;
}

void System::Draw()
{
	if (!screenDirty)
	{
		if (focusedWindowDirty)
		{
			state.currentEvent = SystemEvent::Repaint;

			mouse.RestoreBackgroundPixels();

			WindowManager::RepaintFocusedWindow();

			if (!VirtualKeyboard::IsVisible())
			{
				mouse.Draw();
			}
			state.currentEvent = SystemEvent::None;

			focusedWindowDirty = false;
		}

		VirtualKeyboard::Update(false);
		return;
	}

	VirtualKeyboard::SetCursorScreenLocation(0, 0);

	state.currentEvent = SystemEvent::Repaint;

	mouse.RestoreBackgroundPixels();

	DrawBG();

	HandleEvent(SystemEvent::Repaint);

	//Font::DrawString("qwertyuiop", 0, DISPLAY_HEIGHT - 24, BLACK);
	//Font::DrawString("asdfghjkl", 0, DISPLAY_HEIGHT - 16, BLACK);
	//Font::DrawString(" zxcvbnm", 0, DISPLAY_HEIGHT - 8, BLACK);

	if (!VirtualKeyboard::IsVisible())
	{
		mouse.Draw();
	}

	state.currentEvent = SystemEvent::None;
	screenDirty = false;
	focusedWindowDirty = false;

	VirtualKeyboard::Update(true);
}

void System::DrawBG()
{
	uint8_t* ptr = Platform::GetScreenBuffer();

	/*if (MenuBar::IsVisible())
	{
		// Menu bar
		*ptr++ = 0x7c;
		*ptr++ = 0x7e;
		for (uint8_t n = 2; n < DISPLAY_WIDTH - 2; n++)
		{
			*ptr++ = 0x7f;
		}
		*ptr++ = 0x7e;
		*ptr++ = 0x7c;
	}
	else*/
	{
		*ptr++ = 0x54;
		*ptr++ = 0xaa;
		for (uint8_t x = 2; x < DISPLAY_WIDTH - 2; x += 2)
		{
			*ptr++ = 0x55;
			*ptr++ = 0xaa;
		}
		*ptr++ = 0x54;
		*ptr++ = 0xa8;
	}

	// Dithered background
	for (uint8_t row = 1; row < (DISPLAY_HEIGHT / 8) - 1; row++)
	{
		for (uint8_t x = 0; x < DISPLAY_WIDTH; x += 2)
		{
			*ptr++ = 0x55;
			*ptr++ = 0xaa;
		}
	}

	*ptr++ = 0x15;
	*ptr++ = 0x2a;
	for (uint8_t x = 2; x < DISPLAY_WIDTH - 2; x += 2)
	{
		*ptr++ = 0x55;
		*ptr++ = 0xaa;
	}
	*ptr++ = 0x55;
	*ptr++ = 0x2a;

}

void System::HandleEvent(SystemEvent eventType)
{
	state.currentEvent = eventType;

	WindowManager::HandleEvent(eventType);
	MenuBar::HandleEvent(eventType);

	state.currentEvent = SystemEvent::None;
}

void System::EnterState(State newState, const Element& inStateElement)
{
	if (state.currentState == State::Default)
	{
		state.currentState = newState;
		state.stateElement = inStateElement;
		state.animationTimer = 0;
	}
}

void System::ExitState(State exitingState)
{
	if (exitingState == state.currentState)
	{
		state.currentState = State::Default;
		state.stateElement = Element();
	}
}

#pragma once

#include <stdint.h>
#include "Window.h"
#include "SystemEvent.h"
#include "Element.h"

class System
{
public:
	enum class State : uint8_t
	{
		Default,
		ClickingButton,
		ClickingCloseWindowButton,
		DraggingWindow,
		ScrollingWindow,
		OpeningWindowAnimation,
		ClosingWindowAnimation,
		DraggingSlider,
		OpeningMenu
	};

	struct StateData
	{
		SystemEvent currentEvent;
		State currentState;
		
		Element selectedElement;
		Element stateElement;

		uint8_t animationTimer;
	};

	static void Init();
	static void Tick();
	static void Draw();
	static void MarkScreenDirty();
	static void MarkFocusedWindowDirty() { focusedWindowDirty = true; }

	static void HandleEvent(SystemEvent eventType);
	//static void EnterState(State newState, WindowHandle window = WindowManager::invalidWindowHandle, uint8_t elementIndex = 0);
	static void EnterState(State newState, const Element& stateElement = Element());
	static void ExitState(State exitingState);
	static void MarkEventHandled() { state.currentEvent = SystemEvent::EventHandled; }

	static StateData state;

private:
	static void DrawBG();
	static bool screenDirty;
	static bool focusedWindowDirty;
};



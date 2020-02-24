#pragma once
#include <stdint.h>

enum class SystemEvent : uint8_t
{
	None,
	Repaint,
	MouseDown,
	MouseUp,
	MouseMove,
	Tick,
	OpenWindow,
	CloseWindow,
	MenuItemClicked,
	KeyPressed,
	GatherMenuItems,
	EventHandled
};

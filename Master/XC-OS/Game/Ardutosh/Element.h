#pragma once

#include <stdint.h>

typedef uint8_t WindowHandle;

struct Element
{
	Element() : window(0xff), index(0xff) {}
	Element(WindowHandle inWindow) : window(inWindow) {}
	Element(WindowHandle inWindow, uint8_t inIndex) : window(inWindow), index(inIndex) {}

	WindowHandle window;
	uint8_t index;
};

inline bool operator==(const Element& lhs, const Element& rhs)
{
	return lhs.window == rhs.window && lhs.index == rhs.index;
}

#pragma once
#include <stdint.h>
#include "StringWrapper.h"
#include "SystemEvent.h"
#include "Element.h"

typedef void(*WindowHandler)(class Window* window, SystemEvent event);
typedef uint8_t WindowHandle;
typedef uint16_t MenuBarMask;

enum class WindowType : uint8_t
{
	Closed,
	Desktop,
	DialogBox,
	FullWindow
};

class Window
{
public:
	void Draw();

	bool Button(xString label, int16_t x, int16_t y);
	bool Item(const uint8_t* icon, const xString& label, int16_t x, int16_t y);
	void VerticalScrollBar(uint16_t& current, uint16_t max);
	void Slider(int16_t x, int16_t y, uint8_t w, uint8_t& current, uint8_t min = 0, uint8_t max = 255);
	void Label(const xString& label, int16_t x, int16_t y);
	void Label(int16_t label, int16_t x, int16_t y);
	bool RadioButton(int16_t x, int16_t y, uint8_t w, uint8_t index, uint8_t& selected);
	bool MenuItem(MenuBarMask item);

	void OpenWithAnimation(uint8_t fromX, uint8_t fromY);
	void HandleEvent(SystemEvent eventType);
	bool IsFocused();

	void MarkContentsDirty();

	WindowHandle GetHandle() const;

	WindowType type;
	void* data;
	WindowHandler handler;
	xString title;
	int x, y;
	uint8_t w, h;
	uint8_t originX, originY;

	static constexpr int windowBarHeight = 8;
private:
	bool IsMouseOverCloseButton() const;
	Element GetCurrentElement() const;
};


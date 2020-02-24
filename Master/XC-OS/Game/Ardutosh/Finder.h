#pragma once

#include <stdint.h>
#include "Defines.h"
#include "SystemEvent.h"

class Window;
class xString;
//typedef Window*(*FinderItemHandler)();
typedef void(*WindowHandler)(class Window* window, SystemEvent event);

struct FinderItem
{
	const uint8_t* icon;
	const char label[9];
	WindowHandler handler;
	void* data;
	uint8_t x, y, w, h;

	const uint8_t* GetIcon() const { return (uint8_t*)pgm_read_ptr(&icon); }
	WindowHandler GetHandler() const { return (WindowHandler)pgm_read_ptr(&handler); }
	void* GetData() const { return (void*) pgm_read_ptr(&data); }
	uint8_t GetX() const { return pgm_read_byte(&x); }
	uint8_t GetY() const { return pgm_read_byte(&y); }
	uint8_t GetW() const { return pgm_read_byte(&w); }
	uint8_t GetH() const { return pgm_read_byte(&h); }
};

class Finder
{
public:
	static Window* OpenArduboyFolder();
	//static Window* OpenSysFolder();
	//static Window* OpenAppsFolder();

	//static Window* OpenFinderWindow(const xString& title, const FinderItem* items);
	static Window* OpenItem(const FinderItem* item);
	static void Handler(Window* window, SystemEvent eventType);
};
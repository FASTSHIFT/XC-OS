#pragma once

#include "SystemEvent.h"
#include <stdint.h>

typedef uint16_t MenuBarMask;

enum MenuItemType
{
	Menu_Item_Null = 0,
	Menu_File_Save = 1,
	Menu_File_Close = 2,
	Menu_File_DumpEEPROM = 4,
	Menu_File_Load = 8,

	Menu_Edit_Clear = 32,
	Menu_Edit_SetBaud = 64,

	Menu_Special_Restart = 256,
	Menu_Special_CloseAll = 512,
};

class MenuBar
{
public:
	static bool HandleEvent(SystemEvent eventType);
	static bool IsVisible();
	static MenuItemType GetSelectedMenuItem();

	static constexpr int height = 8;

private:
	static const struct MenuItem* selectedItem;
	static void Draw();
	static const MenuItem* GetItemUnderMouse();
};

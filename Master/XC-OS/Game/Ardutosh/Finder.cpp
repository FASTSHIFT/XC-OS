#include "Finder.h"
#include "WindowManager.h"
#include "Apps.h"
#include "MenuBar.h"
#include "Generated/Sprites.h"

struct GridView
{
	GridView(Window* inWindow, int inStartX = 14, int inStartY = 10, int inSpacingX = 30, int inSpacingY = 22)
		: x(inStartX), y(inStartY), window(inWindow), startX(inStartX), startY(inStartY), spacingX(inSpacingX), spacingY(inSpacingY)
	{
	}

	int x, y;

	void Next()
	{
		x += spacingX;
		if (x >= window->w)
		{
			x = startX;
			y += spacingY;
		}
	}

private:
	Window* window;
	int startX, startY;
	int spacingX, spacingY;
};


/*
const static FinderItem finderItems[] PROGMEM =
{
{ terminalIcon,		"Terminal",		Apps::OpenTerminalApp },
{ eepromIcon,		"EEPROM",		Apps::OpenEEPROMInspectorApp },
{ batteryIcon,		"Battery",		Apps::OpenBatteryApp },
{ temperatureIcon,	"Thermal",		Apps::OpenTemperatureApp },
{ ledIcon,			"LED",			Apps::OpenLEDApp },
{ remoteIcon,		"Remote",		Apps::OpenRemoteApp },
{ documentIcon,		"Readme",		Apps::OpenReadme },
{ nullptr, "" }
};
*/

const static FinderItem finderSysItems[] PROGMEM =
{
	{ eepromIcon,		"EEPROM",		Apps::EEPROMInspectorApp,	nullptr, 0, 8, 128, 54 },
	{ batteryIcon,		"Battery",		Apps::BatteryApp,		nullptr, 0, 8, 85, 54 },
	{ temperatureIcon,	"Thermal",		Apps::TemperatureApp,	nullptr, 0, 8, 85, 54 },
	{ ledIcon,			"LED",			Apps::LEDApp,			nullptr, 8, 10, 74, 52 },
	{ nullptr, "" }
};

const static FinderItem finderAppsItems[] PROGMEM =
{
	{ terminalIcon,		"Terminal",		Apps::TerminalApp,		nullptr, 0, 8, 128, 54 },
	{ remoteIcon,		"Remote",		Apps::RemoteApp,		nullptr, 14, 8, 100, 54, },
	{ notesIcon,		"Notes",		Apps::TextReaderApp,	(void*) Apps::NotesBuffer, 4, 9, 96, 54 },
	{ nullptr, "" }
};

static const FinderItem finderArduboyItems[] PROGMEM =
{
	{ folderIcon,		"Apps",			Finder::Handler,		(void*) finderAppsItems,	8, 16, 96, 34 },
	{ folderIcon,		"Sys",			Finder::Handler,		(void*) finderSysItems,		32, 8, 64, 54 },
	{ documentIcon,		"Readme",		Apps::TextReaderApp,	nullptr, 4, 9, 96, 54 },
	{ nullptr, "" }
};

const static FinderItem arduboyFolder PROGMEM =
{
	arduboyIcon,	"Arduboy",		Finder::Handler,	(void*) finderArduboyItems,		0, 8, 96, 34
};

Window* Finder::OpenItem(const FinderItem* item)
{
	Window* win = nullptr;
	if (item->GetData())
		win = WindowManager::FindByData(item->GetData());
	else
		win = WindowManager::FindByHandler(item->GetHandler());

	if (win)
	{
		WindowManager::Focus(win);
		return nullptr;
	}

	win = WindowManager::Create(WindowType::FullWindow, item->GetHandler(), item->GetData());
	if (win)
	{
		win->title = xString(item->label, xString::Type::Flash);
		win->x = item->GetX();
		win->y = item->GetY();
		win->w = item->GetW();
		win->h = item->GetH();
		return win;
		//win->OpenWithAnimation(itemsX + 5, itemsY + 5);
	}

	return nullptr;
}

Window* Finder::OpenArduboyFolder()
{
	return OpenItem(&arduboyFolder);
}

/*Window* Finder::OpenFinderWindow(const xString& title, const FinderItem* items)
{
	Window* win = WindowManager::FindByData((void*)items);

	if (win)
	{
		WindowManager::Focus(win);
		return nullptr;
	}

	int itemCount = 0;
	for (const FinderItem* item = items; item->GetIcon(); item++)
		itemCount++;

	win = WindowManager::Create(WindowType::FullWindow, Handler, (void*) items);
	if (win)
	{
		win->title = title;
		win->x = 0;
		win->y = 8;
		//win->w = 128;
		if (itemCount == 3)
		{
			win->w = 96;
			win->h = 34;
		}
		else
		{
			win->w = 64;
			win->h = 54;
		}

		win->y = (DISPLAY_HEIGHT - MenuBar::height) / 2 + MenuBar::height - win->h / 2;
		win->x = 96 / 2 - win->w / 2;

		return win;
	}

	return nullptr;
}*/

void Finder::Handler(Window* window, SystemEvent eventType)
{
	GridView grid(window);

	for (const FinderItem* item = (const FinderItem*)(window->data); item->GetIcon(); item++)
	{
		if (window->Item(item->GetIcon(), xString(item->label, xString::Type::Flash), grid.x, grid.y))
		{
			if (Window* win = OpenItem(item))
			{
				win->OpenWithAnimation(grid.x + 5, grid.y + 5);
			}
		}
		grid.Next();
	}
}

#include "MenuBar.h"
#include "WindowManager.h"
#include "Font.h"
#include "StringWrapper.h"
#include "Input.h"
#include "Platform.h"
#include "System.h"
#include "VirtualKeyboard.h"

struct MenuItem
{
	static constexpr int maxTitleLength = 12;
	const char title[maxTitleLength];
	MenuItemType mask;

	bool IsRoot() const
	{
		return GetMask() == 0;
	}
	bool IsEnd() const
	{
		return GetTitle().Length() == 0;
	}
	const MenuItem* GetRoot() const
	{
		const MenuItem* item = this;
		while (!item->IsRoot())
			item--;
		return item;
	}

	xString GetTitle() const
	{
		return xString(title, xString::Type::Flash);
	}
	MenuItemType GetMask() const
	{
		return (MenuItemType) pgm_read_word(&mask);
	}

	bool IsSelected(const MenuItem* selectedItem) const
	{
		if (!selectedItem)
			return false;
		if (this == selectedItem)
			return true;
		if (IsRoot())
		{
			for (const MenuItem* item = this + 1; !item->IsRoot() && !item->IsEnd(); ++item)
			{
				if (item == selectedItem)
					return true;
			}
		}
		return false;
	}

	bool IsActive(uint16_t activeMask) const
	{
		if (IsRoot())
		{
			for (const MenuItem* item = this + 1; !item->IsRoot() && !item->IsEnd(); ++item)
			{
				if (item->IsActive(activeMask))
					return true;
			}
		}
		else
		{
			return (GetMask() & activeMask) != 0;
		}
		return false;
	}

	int CountActiveItems(uint16_t activeMask) const
	{
		if (!IsRoot())
			return 0;

		int count = 0;

		for (const MenuItem* item = this + 1; !item->IsRoot() && !item->IsEnd(); ++item)
		{
			if (item->IsActive(activeMask))
				count++;
		}

		return count;
	}
};

const MenuItem menuItems[] PROGMEM =
{
	{ "File" },
		{ "Load",			Menu_File_Load },
		{ "Save",			Menu_File_Save },
		{ "Dump EEPROM",	Menu_File_DumpEEPROM },
		{ "Close",			Menu_File_Close },

	{ "Edit" },
		{ "Clear",			Menu_Edit_Clear },
		{ "Set baud",		Menu_Edit_SetBaud },

	{ "Special" },
		{ "Close all",		Menu_Special_CloseAll },
		{ "Restart",		Menu_Special_Restart },

	{ "" }
};

constexpr int itemStartX = 10;
constexpr int itemStartY = 1;
constexpr int itemSpacing = 6;
constexpr int itemVerticalSpacing = 7;
constexpr int itemPadding = 2;
constexpr int menuWidth = 52;

const MenuItem* MenuBar::selectedItem = nullptr;
void MenuBar::Draw()
{
	if (!IsVisible())
		return;

	uint16_t activeMask = WindowManager::GetMenuBarItemMask();

	int menuX = itemStartX;
	int menuRootY = 0;
	int dummyX = 0;

	VirtualKeyboard::ApplyScreenShift(dummyX, menuRootY);

	Platform::FillRect(0, menuRootY, DISPLAY_WIDTH, MenuBar::height - 1, WHITE);
	// Corners
	Platform::DrawFastVLine(0, menuRootY, 2, BLACK);
	Platform::DrawPixel(1, menuRootY, BLACK);
	Platform::DrawFastVLine(DISPLAY_WIDTH - 1, menuRootY, 2, BLACK);
	Platform::DrawPixel(DISPLAY_WIDTH - 2, menuRootY, BLACK);
	// Outline
	Platform::DrawFastHLine(0, menuRootY + MenuBar::height - 1, DISPLAY_WIDTH, BLACK);

	for (const MenuItem* item = menuItems; !item->IsEnd(); ++item)
	{
		if (item->IsRoot())
		{
			if (item->IsActive(activeMask))
			{
				bool isSelected = item->IsSelected(selectedItem);
				int itemWidth = item->GetTitle().Length() * Font::glyphWidth + itemPadding * 2;

				if (isSelected)
				{
					Platform::FillRect(menuX, menuRootY, itemWidth, height, BLACK);
				}

				Font::DrawString(item->GetTitle(), menuX + itemPadding, menuRootY + itemStartY, isSelected ? WHITE : BLACK);

				if (isSelected)
				{
					int numChildItems = item->CountActiveItems(activeMask);
					int menuHeight = itemVerticalSpacing * numChildItems + itemPadding;
					int menuY = menuRootY + MenuBar::height;

					Platform::FillRect(menuX, menuY, menuWidth, menuHeight, WHITE);
					Platform::DrawRect(menuX, menuY, menuWidth, menuHeight, BLACK);
					Platform::DrawFastVLine(menuX + menuWidth, menuY + 1, menuHeight, BLACK);
					Platform::DrawFastHLine(menuX + 1, menuY + menuHeight, menuWidth, BLACK);

					int itemY = menuY + itemPadding;

					while (numChildItems > 0)
					{
						item++;
						if (item->IsActive(activeMask))
						{
							numChildItems--;

							if (item->IsSelected(selectedItem))
							{
								Platform::FillRect(menuX, itemY - itemPadding, menuWidth, itemVerticalSpacing + 1, BLACK);
								Font::DrawString(item->GetTitle(), menuX + itemSpacing, itemY, WHITE);
							}
							else
							{
								Font::DrawString(item->GetTitle(), menuX + itemSpacing, itemY, BLACK);
							}
							itemY += itemVerticalSpacing;
						}
					}
				}

				menuX += itemWidth + itemSpacing;
			}
		}
	}
}

bool MenuBar::IsVisible()
{
	uint16_t activeMask = WindowManager::GetMenuBarItemMask();
	int count = 0;

	for (const MenuItem* item = menuItems; !item->IsEnd(); ++item)
	{
		if (item->IsRoot() && item->IsActive(activeMask))
		{
			count++;
		}
	}

	return count > 0;
}

const MenuItem* MenuBar::GetItemUnderMouse()
{
	uint16_t activeMask = WindowManager::GetMenuBarItemMask();
	int menuX = itemStartX;

	for (const MenuItem* item = menuItems; !item->IsEnd(); ++item)
	{
		if (item->IsRoot() && item->IsActive(activeMask))
		{
			int itemWidth = item->GetTitle().Length() * Font::glyphWidth + itemPadding * 2;
			if (mouse.x >= menuX && mouse.x < menuX + itemWidth && mouse.y < MenuBar::height)
			{
				return item;
			}

			if (item->IsSelected(selectedItem))
			{
				int numChildItems = item->CountActiveItems(activeMask);
				int menuHeight = itemVerticalSpacing * numChildItems + itemPadding;
				int menuY = MenuBar::height;

				if (mouse.x >= menuX && mouse.x < menuX + menuWidth && mouse.y > MenuBar::height && mouse.y < menuY + menuHeight)
				{
					int childIndex = (mouse.y - menuY) / itemVerticalSpacing;
					if (childIndex >= numChildItems)
					{
						return item;
					}

					while (childIndex >= 0)
					{
						++item;
						if (item->IsActive(activeMask))
						{
							if (childIndex == 0)
							{
								return item;
							}
							else childIndex--;
						}
					}
				}
			}			

			menuX += itemWidth + itemSpacing;
		}
	}

	return nullptr;
}

bool MenuBar::HandleEvent(SystemEvent eventType)
{
	if (eventType == SystemEvent::Repaint)
	{
		Draw();
	}
	else if (eventType == SystemEvent::MouseDown && !WindowManager::ShowingDialog())
	{
		selectedItem = GetItemUnderMouse();
		if (selectedItem)
		{
			System::EnterState(System::State::OpeningMenu);
			System::MarkScreenDirty();
		}
	}
	else if (eventType == SystemEvent::MouseMove)
	{
		if (System::state.currentState == System::State::OpeningMenu)
		{
			const MenuItem* under = GetItemUnderMouse();
			if (under == nullptr)
			{
				if (!selectedItem->IsRoot())
				{
					selectedItem = selectedItem->GetRoot();
					System::MarkScreenDirty();
				}
			}
			else if (under != selectedItem)
			{
				selectedItem = under;
				System::MarkScreenDirty();
			}
			
		}
	}
	else if (eventType == SystemEvent::MouseUp)
	{
		if (System::state.currentState == System::State::OpeningMenu)
		{
			selectedItem = GetItemUnderMouse();
			if (selectedItem && !selectedItem->IsRoot())
			{
				System::HandleEvent(SystemEvent::MenuItemClicked);
			}
			System::ExitState(System::State::OpeningMenu);
			System::MarkScreenDirty();
			selectedItem = nullptr;
		}
	}
	return false;
}

MenuItemType MenuBar::GetSelectedMenuItem()
{
	if (selectedItem)
	{
		return selectedItem->GetMask();
	}
	return Menu_Item_Null;
}

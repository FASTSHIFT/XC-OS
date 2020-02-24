#include "Desktop.h"
#include "Defines.h"
#include "Platform.h"
#include "Font.h"
#include "Input.h"
#include "WindowManager.h"
#include "Generated/Sprites.h"
#include "Apps.h"
#include "Finder.h"
#include "MenuBar.h"
#include "VirtualKeyboard.h"

void* finderHandle = (void*)1;
void* trashFinderHandle = (void*)2;

void Desktop::Handler(Window* window, SystemEvent event)
{
    constexpr int padding = 2;
    constexpr int itemWidth = 18;
    constexpr int itemHeight = 18;

    constexpr int itemsX = DISPLAY_WIDTH - itemWidth - padding;
    constexpr int itemsY = MenuBar::height + padding;

    if (window->MenuItem(Menu_File_Close))
    {

    }

    if (window->Item(arduboyIcon, FlashString("Arduboy"), itemsX, itemsY))
    {
        if (Window* win = Finder::OpenArduboyFolder())
        {
            win->OpenWithAnimation(itemsX + 5, itemsY + 5);
        }
    }
    /*if (window->Item(trashEmptyIcon, FlashString("Trash"), itemsX, DISPLAY_HEIGHT - itemHeight - padding))
    {
        Window* win = WindowManager::FindByData(trashFinderHandle);

        if (win)
        {
            WindowManager::Focus(win);
        }
        else
        {
            win = WindowManager::Create(WindowType::FullWindow, nullptr);
            if (win)
            {
                win->title = "Trash";
                win->x = 20;
                win->y = 15;
                win->w = 60;
                win->h = 40;
                win->data = trashFinderHandle;
                win->OpenWithAnimation(itemsX + 5, DISPLAY_HEIGHT - itemHeight - padding + 5);
            }
        }
    }*/

    /*if (event == SystemEvent::KeyPressed)
    {
        static bool enabled = false;
        if (!enabled)
        {
            enabled = true;
            PlatformRemote::SetKeyboardEnabled(true);
        }
        PlatformRemote::KeyboardWrite(VirtualKeyboard::GetLastKeyPressed());
    }*/
}

void Desktop::CreateDesktop()
{
    Window* desktop = WindowManager::Create(WindowType::Desktop, Desktop::Handler);
    desktop->x = desktop->y = 0;
}

void Desktop::ShowSplash()
{
    constexpr int splashWidth = 108;
    constexpr int splashHeight = 36;
    constexpr int splashX = DISPLAY_WIDTH / 2 - splashWidth / 2;
    constexpr int splashY = DISPLAY_HEIGHT / 2 - splashHeight / 2;

    Window* splash = WindowManager::Create(WindowType::DialogBox, Desktop::SplashHandler);
    splash->x = splashX;
    splash->y = splashY;
    splash->w = splashWidth;
    splash->h = splashHeight;
}

void Desktop::SplashHandler(Window* window, SystemEvent event)
{
    if (event == SystemEvent::Repaint)
    {
        Platform::DrawSprite(17, DISPLAY_HEIGHT / 2 - 6, arduboyIcon, 0);
        Font::DrawString(FlashString("Welcome to Arduboy."), 31, DISPLAY_HEIGHT / 2 - Font::glyphHeight / 2, BLACK);
    }

    constexpr int splashShowTime = 64;
    if (System::state.animationTimer > splashShowTime)
    {
        WindowManager::Destroy(window);
        CreateDesktop();
    }
}

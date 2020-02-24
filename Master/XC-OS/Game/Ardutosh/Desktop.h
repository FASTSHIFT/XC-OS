#pragma once
#include <stdint.h>
#include "StringWrapper.h"
#include "System.h"

class Desktop
{
public:
	static void Handler(class Window* window, SystemEvent event);
	static void CreateDesktop();

	static void SplashHandler(class Window* window, SystemEvent event);
	static void ShowSplash();
};
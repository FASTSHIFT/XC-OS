#pragma once

#include "SystemEvent.h"

class Window;
class xString;

class Apps
{
public:
	static void TerminalApp(Window* window, SystemEvent eventType);
	static void FinderApp(Window* window, SystemEvent eventType);
	static void TextReaderApp(Window* window, SystemEvent eventType);
	static void BatteryApp(Window* window, SystemEvent eventType);
	static void TemperatureApp(Window* window, SystemEvent eventType);
	static void LEDApp(Window* window, SystemEvent eventType);
	static void EEPROMInspectorApp(Window* window, SystemEvent eventType);
	static void RemoteApp(Window* window, SystemEvent eventType);

	static void SetBaudRateDialog(Window* window, SystemEvent eventType);

	static constexpr int maxNotesBufferSize = 400;
	static char NotesBuffer[maxNotesBufferSize + 1];
};

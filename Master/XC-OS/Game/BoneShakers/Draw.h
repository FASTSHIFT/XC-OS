#pragma once

#include <stdint.h>

enum class MessageType : uint8_t
{
	None,
	Ready,
	Set,
	Go,
	Lap2,
	Lap3,
	WrongWay,
	FirstPlace,
	SecondPlace,
	ThirdPlace,
	FourthPlace
};

void Render(void);
void DrawTitle(void);
void ShowMessage(MessageType messageType);

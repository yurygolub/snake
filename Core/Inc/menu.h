#pragma once
#include "main.h"

typedef struct
{
	void (*handler)();
	char* name;
}MenuItem;

void DisplayMenu();

void ChooseMenuItem(const MenuItem* items, const uint8_t itemsCount,
		uint16_t backgroundColor, uint16_t textColor, uint16_t cursorColor);

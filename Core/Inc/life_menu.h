#pragma once
#include "main.h"
#include "menu.h"
#include "life.h"

void LifeMenu();

static const MenuItem lifeMenuItems[] =
{
	{ .name = "Play", .handler = LifeGame },
};

#define NUMBER_OF_LIFE_MENU_ITEMS (sizeof(lifeMenuItems) / sizeof(MenuItem))

void LifeMenu()
{
	ChooseMenuItem(lifeMenuItems, NUMBER_OF_LIFE_MENU_ITEMS,
			LCD_COLOR_DARKGRAY, LCD_COLOR_DARKYELLOW, LCD_COLOR_RED);
}

#undef NUMBER_OF_LIFE_MENU_ITEMS

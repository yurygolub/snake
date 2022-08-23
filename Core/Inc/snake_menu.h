#pragma once
#include "main.h"
#include "menu.h"
#include "snake.h"

void SnakeMenu();
static void SnakeSettingsMenu();
static void SnakeSettingEasy();
static void SnakeSettingMedium();
static void SnakeSettingHard();

const MenuItem snakeMenuItems[] =
{
	{ .name = "Play", .handler = SnakeGame },
	{ .name = "Settings", .handler = SnakeSettingsMenu },
};

#define NUMBER_OF_SNAKE_MENU_ITEMS (sizeof(snakeMenuItems) / sizeof(MenuItem))

static const MenuItem snakeSettingMenuItems[] =
{
	{ .name = "Easy", .handler = SnakeSettingEasy },
	{ .name = "Medium", .handler = SnakeSettingMedium },
	{ .name = "Hard", .handler = SnakeSettingHard },
};

#define NUMBER_OF_SNAKE_SETTING_MENU_ITEMS (sizeof(snakeSettingMenuItems) / sizeof(MenuItem))

void SnakeMenu()
{
	ChooseMenuItem(snakeMenuItems, NUMBER_OF_SNAKE_MENU_ITEMS,
			LCD_COLOR_GRAY, LCD_COLOR_DARKMAGENTA, LCD_COLOR_DARKBLUE);
}

static void SnakeSettingsMenu()
{
	ChooseMenuItem(snakeSettingMenuItems, NUMBER_OF_SNAKE_SETTING_MENU_ITEMS,
			LCD_COLOR_GRAY, LCD_COLOR_DARKMAGENTA, LCD_COLOR_DARKBLUE);
}

static void SnakeSettingEasy()
{
	Setup(20, 50);
}

static void SnakeSettingMedium()
{
	Setup(15, 100);
}

static void SnakeSettingHard()
{
	Setup(10, 200);
}

#undef NUMBER_OF_SNAKE_SETTING_MENU_ITEMS

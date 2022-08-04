#include "menu.h"

extern volatile bool timerFlag;

static bool rising = true;
static uint8_t currentRadius = 0;

static void Easy();
static void Medium();
static void Hard();
static void SettingsMenu();
static void DisplayMenuItems(const MenuItem* items, const uint8_t itemsCount);
static void ChooseMenuItem(const MenuItem* items, const uint8_t itemsCount);

static JOYState_TypeDef JoysticPosition(const uint8_t currentPos, const uint8_t numberOfItems);
static void EraseCursor(uint8_t xPos, uint8_t yPos, uint8_t radius);
static void DrawCursor(uint8_t xPos, uint8_t yPos, uint8_t radius);

static const MenuItem menuItems[] =
{
	{ .name = "Play", .func = SnakeGame},
	{ .name = "Life", .func = LifeGame},
	{ .name = "Settings", .func = SettingsMenu},
};

#define NUMBER_OF_MENU_ITEMS (sizeof(menuItems) / sizeof(MenuItem))

static const MenuItem settingMenuItems[] =
{
	{ .name = "Easy", .func = Easy},
	{ .name = "Medium", .func = Medium},
	{ .name = "Hard", .func = Hard},
};

#define NUMBER_OF_SETTING_MENU_ITEMS (sizeof(settingMenuItems) / sizeof(MenuItem))

void DisplayMenu()
{
	ChooseMenuItem(menuItems, NUMBER_OF_MENU_ITEMS);
}

static void Easy()
{
	Setup(20, 50);
}

static void Medium()
{
	Setup(15, 100);
}

static void Hard()
{
	Setup(10, 200);
}

static void SettingsMenu()
{
	ChooseMenuItem(settingMenuItems, NUMBER_OF_SETTING_MENU_ITEMS);
}

static void DisplayMenuItems(const MenuItem* items, const uint8_t itemsCount)
{
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	for (size_t i = 0; i < itemsCount; i++)
	{
		BSP_LCD_DisplayStringAt(0, Font24.Height * (i + 1),
				(uint8_t *)items[i].name, LEFT_MODE);
	}
}

static void ChooseMenuItem(const MenuItem* items, const uint8_t itemsCount)
{
	DisplayMenuItems(items, itemsCount);

	uint8_t xStart = 200;
	uint8_t yStart = 36;
	uint8_t radius = 6;
	uint8_t currentPos = 0;
	uint16_t cursorCounter = 0;
	uint16_t joyCounter = 0;

	while (true)
	{
		if (timerFlag)
		{
			timerFlag = false;
			cursorCounter++;
			joyCounter++;
		}

		if (cursorCounter == 40)
		{
			cursorCounter = 0;
			DrawCursor(xStart, yStart + Font24.Height * currentPos, radius);
		}

		if (joyCounter == 200)
		{
			joyCounter = 0;
			JOYState_TypeDef joyState = JoysticPosition(currentPos, itemsCount);
			switch (joyState)
			{
			case JOY_UP:
				EraseCursor(xStart, yStart + Font24.Height * currentPos, radius);
				currentPos--;
				break;

			case JOY_DOWN:
				EraseCursor(xStart, yStart + Font24.Height * currentPos, radius);
				currentPos++;
				break;

			case JOY_SEL:
				if (items[currentPos].func != NULL)
				{
					items[currentPos].func();
				}

				return;
			}
		}
	}
}

static JOYState_TypeDef JoysticPosition(const uint8_t currentPos, const uint8_t numberOfItems)
{
	if (HAL_GPIO_ReadPin(JOY_UP_GPIO_Port, JOY_UP_Pin) && currentPos > 0)
	{
		return JOY_UP;
	}
	else if (HAL_GPIO_ReadPin(JOY_DOWN_GPIO_Port, JOY_DOWN_Pin) && currentPos < numberOfItems - 1)
	{
		return JOY_DOWN;
	}
	else if (HAL_GPIO_ReadPin(JOY_SEL_GPIO_Port, JOY_SEL_Pin))
	{
		return JOY_SEL;
	}

	return JOY_NONE;
}

static void EraseCursor(uint8_t xPos, uint8_t yPos, uint8_t radius)
{
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillCircle(xPos, yPos, radius);
}

static void DrawCursor(uint8_t xPos, uint8_t yPos, uint8_t radius)
{
	if (rising)
	{
		if (currentRadius < radius)
		{
			currentRadius++;
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(xPos, yPos, currentRadius);
		}
		else
		{
			rising = false;
		}
	}
	else
	{
		if (currentRadius > 0)
		{
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DrawCircle(xPos, yPos, currentRadius);
			currentRadius--;
		}
		else
		{
			rising = true;
		}
	}
}

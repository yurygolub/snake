#include "menu.h"
#include "snake_menu.h"
#include "life_menu.h"

extern volatile bool timerFlag;

static bool rising = true;
static uint8_t currentRadius = 0;

static void DisplayMenuItems(const MenuItem* items, const uint8_t itemsCount,
		uint16_t backgroundColor, uint16_t textColor);

static JOYState_TypeDef JoysticPosition(const uint8_t currentPos, const uint8_t numberOfItems);
static void EraseCursor(uint8_t xPos, uint8_t yPos, uint8_t radius,
		uint16_t backgroundColor);
static void DrawCursor(uint8_t xPos, uint8_t yPos, uint8_t radius,
		uint16_t color, uint16_t backgroundColor);

static const MenuItem globalMenuItems[] =
{
	{ .name = "Snake", .handler = SnakeMenu },
	{ .name = "Life", .handler = LifeMenu },
};

#define NUMBER_OF_GLOBAL_MENU_ITEMS (sizeof(globalMenuItems) / sizeof(MenuItem))

void DisplayMenu()
{
	ChooseMenuItem(globalMenuItems, NUMBER_OF_GLOBAL_MENU_ITEMS,
			LCD_COLOR_GRAY, LCD_COLOR_BLACK, LCD_COLOR_DARKRED);
}

static void DisplayMenuItems(const MenuItem* items, const uint8_t itemsCount,
		uint16_t backgroundColor, uint16_t textColor)
{
	BSP_LCD_Clear(backgroundColor);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_SetTextColor(textColor);
	BSP_LCD_SetBackColor(backgroundColor);
	for (size_t i = 0; i < itemsCount; i++)
	{
		BSP_LCD_DisplayStringAt(0, Font24.Height * (i + 1),
				(uint8_t *)items[i].name, LEFT_MODE);
	}
}

void ChooseMenuItem(const MenuItem* items, const uint8_t itemsCount,
		uint16_t backgroundColor, uint16_t textColor, uint16_t cursorColor)
{
	DisplayMenuItems(items, itemsCount, backgroundColor, textColor);

	uint8_t xStart = 200;
	uint8_t yStart = 36;
	uint8_t radius = 8;
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
			DrawCursor(xStart, yStart + Font24.Height * currentPos, radius,
					cursorColor, backgroundColor);
		}

		if (joyCounter == 200)
		{
			joyCounter = 0;
			JOYState_TypeDef joyState = JoysticPosition(currentPos, itemsCount);
			switch (joyState)
			{
			case JOY_UP:
				EraseCursor(xStart, yStart + Font24.Height * currentPos, radius,
						backgroundColor);
				currentPos--;
				break;

			case JOY_DOWN:
				EraseCursor(xStart, yStart + Font24.Height * currentPos, radius,
						backgroundColor);
				currentPos++;
				break;

			case JOY_SEL:
				if (items[currentPos].handler != NULL)
				{
					items[currentPos].handler();
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

static void EraseCursor(uint8_t xPos, uint8_t yPos, uint8_t radius,
		uint16_t backgroundColor)
{
	BSP_LCD_SetTextColor(backgroundColor);
	BSP_LCD_FillCircle(xPos, yPos, radius);
	rising = true;
	currentRadius = 0;
}

static void DrawCursor(uint8_t xPos, uint8_t yPos, uint8_t radius,
		uint16_t color, uint16_t backgroundColor)
{
	if (rising)
	{
		if (currentRadius < radius)
		{
			currentRadius++;
			BSP_LCD_SetTextColor(color);
			BSP_LCD_FillCircle(xPos, yPos, currentRadius);
		}
		else
		{
			rising = false;
		}
	}
	else
	{
		if (currentRadius > 1)
		{
			BSP_LCD_SetTextColor(backgroundColor);
			BSP_LCD_DrawCircle(xPos, yPos, currentRadius);
			currentRadius--;
		}
		else
		{
			rising = true;
		}
	}
}

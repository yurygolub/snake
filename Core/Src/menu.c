#include "menu.h"

static void Test1();
static void Test2();
static const MenuItem menuItems[] =
{
	{ .name = "Snake", .func = SnakeGame},
	{ .name = "Test1", .func = Test1 },
	{ .name = "Test2", .func = Test2 },
};

#define NUMBER_OF_MENU_ITEMS (sizeof(menuItems) / sizeof(MenuItem))

void DisplayMenuItems()
{
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	//BSP_LCD_SetBackColor(LCD_COLOR_GRAY);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	for (size_t i = 0; i < NUMBER_OF_MENU_ITEMS; i++)
	{
		BSP_LCD_DisplayStringAt(0, Font24.Height * (i + 1),
				(uint8_t *)menuItems[i].name, LEFT_MODE);
	}
}

void ChooseMenuItem()
{
	uint8_t xStart = 200;
	uint8_t yStart = 36 ;
	uint8_t radius = 6;
	uint8_t currentPos = 0;

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillCircle(xStart, yStart, radius);

	while (true)
	{
		if (HAL_GPIO_ReadPin(JOY_UP_GPIO_Port, JOY_UP_Pin) && currentPos > 0)
		{
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(xStart, yStart + Font24.Height * currentPos, radius);
			currentPos--;
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(xStart, yStart + Font24.Height * currentPos, radius);
		}
		else if (HAL_GPIO_ReadPin(JOY_DOWN_GPIO_Port, JOY_DOWN_Pin) && currentPos < NUMBER_OF_MENU_ITEMS - 1)
		{
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillCircle(xStart, yStart + Font24.Height * currentPos, radius);
			currentPos++;
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(xStart, yStart + Font24.Height * currentPos, radius);
		}
		else if (HAL_GPIO_ReadPin(JOY_SEL_GPIO_Port, JOY_SEL_Pin))
		{
			if (menuItems[currentPos].func != NULL)
			{
				menuItems[currentPos].func();
			}

			HAL_Delay(200);
			return;
		}

		HAL_Delay(200);
	}
}

static void Test1()
{
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
	BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Test1", LEFT_MODE);
	HAL_Delay(200);
	while (!HAL_GPIO_ReadPin(JOY_SEL_GPIO_Port, JOY_SEL_Pin));
}

static void Test2()
{
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"Test2", CENTER_MODE);
	HAL_Delay(200);
	while (!HAL_GPIO_ReadPin(JOY_SEL_GPIO_Port, JOY_SEL_Pin));
}

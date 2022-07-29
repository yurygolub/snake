#include "snake.h"

static size_t snakeSize;
static struct Point snakeBody[100];
static const int16_t pointSize = 10;

void SnakeInit()
{
	snakeSize = 10;
	int16_t xStart = 1;
	int16_t yStart = BSP_LCD_GetYSize() / pointSize / 2;
	for (size_t i = 0; i < snakeSize; i++)
	{
		struct Point point = { xStart + i, yStart };
		snakeBody[i] = point;
	}
}

void Move(enum Direction direction)
{
	struct Point tail = snakeBody[0];
	for (size_t i = 0; i < snakeSize - 1; i++)
	{
		snakeBody[i] = snakeBody[i + 1];
	}

	switch (direction)
	{
	case Up:
		if (snakeBody[snakeSize - 1].y - 1 >= 0)
		{
			snakeBody[snakeSize - 1].y--;
		}
		else
		{
			snakeBody[snakeSize - 1].y = BSP_LCD_GetYSize() / pointSize - 1;
		}

		break;

	case Down:
		if ((snakeBody[snakeSize - 1].y + 1) * pointSize < BSP_LCD_GetYSize())
		{
			snakeBody[snakeSize - 1].y++;
		}
		else
		{
			snakeBody[snakeSize - 1].y = 0;
		}

		break;

	case Right:
		if ((snakeBody[snakeSize - 1].x + 1) * pointSize < BSP_LCD_GetXSize())
		{
			snakeBody[snakeSize - 1].x++;
		}
		else
		{
			snakeBody[snakeSize - 1].x = 0;
		}

		break;

	case Left:
		if (snakeBody[snakeSize - 1].x - 1 >= 0)
		{
			snakeBody[snakeSize - 1].x--;
		}
		else
		{
			snakeBody[snakeSize - 1].x = BSP_LCD_GetXSize() / pointSize - 1;
		}

		break;
	}

	DrawPoint(tail, LCD_COLOR_WHITE);
	DrawPoint(snakeBody[snakeSize - 1], LCD_COLOR_GREEN);
}

void DrawSnake()
{
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	for (size_t i = 0; i < snakeSize; i++)
	{
		DrawPoint(snakeBody[i], LCD_COLOR_GREEN);
	}
}

void DrawPoint(struct Point point, uint16_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillRect(point.x * pointSize, point.y * pointSize, pointSize, pointSize);
}

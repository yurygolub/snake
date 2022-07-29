#include "snake.h"

#define BUFFER_SIZE 15

static size_t snakeSize;
static struct Point snakeBody[BUFFER_SIZE];
static const int16_t pointSize = 10;
static struct Point apple;

int16_t score;

void SnakeInit()
{
	score = 0;
	snakeSize = 10;
	int16_t xStart = 1;
	int16_t yStart = BSP_LCD_GetYSize() / pointSize / 2;
	for (size_t i = 0; i < snakeSize; i++)
	{
		struct Point point = { xStart + i, yStart };
		snakeBody[i] = point;
	}

	GenerateApple();
}

enum GameState Move(enum Direction direction)
{
	struct Point head = snakeBody[snakeSize - 1];
	switch (direction)
	{
	case Up:
		if (head.y - 1 >= 0)
		{
			head.y--;
		}
		else
		{
			head.y = BSP_LCD_GetYSize() / pointSize - 1;
		}

		break;

	case Down:
		if ((head.y + 1) * pointSize < BSP_LCD_GetYSize())
		{
			head.y++;
		}
		else
		{
			head.y = 0;
		}

		break;

	case Right:
		if ((head.x + 1) * pointSize < BSP_LCD_GetXSize())
		{
			head.x++;
		}
		else
		{
			head.x = 0;
		}

		break;

	case Left:
		if (head.x - 1 >= 0)
		{
			head.x--;
		}
		else
		{
			head.x = BSP_LCD_GetXSize() / pointSize - 1;
		}

		break;
	}

	int8_t eat = 0;
	if (IsEqual(head, apple))
	{
		eat = 1;
		score++;
		snakeSize++;
		snakeBody[snakeSize - 1] = head;

		DrawPoint(snakeBody[snakeSize - 1], LCD_COLOR_GREEN);

		if (snakeSize == BUFFER_SIZE)
		{
			return Winning;
		}

		GenerateApple();
		DrawApple();
	}

	if (!eat)
	{
		struct Point tail = snakeBody[0];
		for (size_t i = 0; i < snakeSize - 1; i++)
		{
			snakeBody[i] = snakeBody[i + 1];
		}

		for (int i = 0; i < snakeSize - 1; i++)
		{
			if (IsEqual(head, snakeBody[i]))
			{
				return Defeat;
			}
		}

		snakeBody[snakeSize - 1] = head;

		DrawPoint(tail, LCD_COLOR_WHITE);
		DrawPoint(snakeBody[snakeSize - 1], LCD_COLOR_GREEN);
	}

	return None;
}

void DrawSnake()
{
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	for (size_t i = 0; i < snakeSize; i++)
	{
		DrawPoint(snakeBody[i], LCD_COLOR_GREEN);
	}
}

void DrawApple()
{
	DrawPoint(apple, LCD_COLOR_RED);
}

void DrawPoint(struct Point point, uint16_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillRect(point.x * pointSize, point.y * pointSize, pointSize, pointSize);
}

void GameOver()
{
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0, 110, (uint8_t *)"Game over", CENTER_MODE);
}

void Win()
{
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0, 110, (uint8_t *)"You win!", CENTER_MODE);
}

void GenerateApple()
{
	int16_t xSize = BSP_LCD_GetXSize() / pointSize;
	int16_t ySize = BSP_LCD_GetYSize() / pointSize;
	while (1)
	{
		int8_t generate = 0;
		apple.x = rand() % xSize;
		apple.y = rand() % ySize;
		for (size_t i = 0; i < snakeSize; i++)
		{
			if (IsEqual(apple, snakeBody[i]))
			{
				generate = 1;
				break;
			}
		}

		if (!generate)
		{
			return;
		}
	}
}

int8_t IsEqual(struct Point point1, struct Point point2)
{
	if (point1.x == point2.x && point1.y == point2.y)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

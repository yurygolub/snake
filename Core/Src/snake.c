#include "snake.h"

#define BUFFER_SIZE 25

static size_t snakeSize;
static Point snakeBody[BUFFER_SIZE];
static const int16_t pointSize = 10;
static Point apple;

extern RNG_HandleTypeDef hrng;

int16_t score;

void SnakeInit()
{
	score = 0;
	snakeSize = 10;
	int16_t xStart = 1;
	int16_t yStart = BSP_LCD_GetYSize() / pointSize / 2;
	for (size_t i = 0; i < snakeSize; i++)
	{
		Point point = { xStart + i, yStart };
		snakeBody[i] = point;
	}

	GenerateApple();
}

GameState Move(Direction direction)
{
	Point head = snakeBody[snakeSize - 1];
	switch (direction)
	{
	case Up:
		if (head.Y - 1 >= 0)
		{
			head.Y--;
		}
		else
		{
			head.Y = BSP_LCD_GetYSize() / pointSize - 1;
		}

		break;

	case Down:
		if ((head.Y + 1) * pointSize < BSP_LCD_GetYSize())
		{
			head.Y++;
		}
		else
		{
			head.Y = 0;
		}

		break;

	case Right:
		if ((head.X + 1) * pointSize < BSP_LCD_GetXSize())
		{
			head.X++;
		}
		else
		{
			head.X = 0;
		}

		break;

	case Left:
		if (head.X - 1 >= 0)
		{
			head.X--;
		}
		else
		{
			head.X = BSP_LCD_GetXSize() / pointSize - 1;
		}

		break;
	}

	bool eat = false;
	if (IsEqual(head, apple))
	{
		eat = true;
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
		Point tail = snakeBody[0];
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

void DrawPoint(Point point, uint16_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillRect(point.X * pointSize, point.Y * pointSize, pointSize, pointSize);
}

void GameOver()
{
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	char result[32];
	snprintf(result, 32, "Game over: %d", score);

	BSP_LCD_DisplayStringAt(0, 110, (uint8_t *)result, CENTER_MODE);
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
	while (true)
	{
		bool isCollide = false;
		uint32_t randomNumber;

		HAL_RNG_GenerateRandomNumber(&hrng, &randomNumber);
		apple.X = randomNumber % xSize;

		HAL_RNG_GenerateRandomNumber(&hrng, &randomNumber);
		apple.Y = randomNumber % ySize;

		for (size_t i = 0; i < snakeSize; i++)
		{
			if (IsEqual(apple, snakeBody[i]))
			{
				isCollide = true;
				break;
			}
		}

		if (!isCollide)
		{
			return;
		}
	}
}

bool IsEqual(Point point1, Point point2)
{
	if (point1.X == point2.X && point1.Y == point2.Y)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

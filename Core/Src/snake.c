#include "snake.h"

#define BUFFER_SIZE 50

static size_t snakeSize;
static Point snakeBody[BUFFER_SIZE];
static const int16_t pointSize = 20;
static Point apple;

extern RNG_HandleTypeDef hrng;

int16_t score;

static void GenerateApple();
static void DrawSnake();
static void DrawApple();
static void DrawPoint(Point point, uint16_t innerColor, uint16_t borderColor);
static bool IsEqual(Point point1, Point point2);

void SnakeInit()
{
	score = 0;
	snakeSize = 5;
	int16_t xStart = 1;
	int16_t yStart = BSP_LCD_GetYSize() / pointSize / 2;
	for (size_t i = 0; i < snakeSize; i++)
	{
		snakeBody[i] = (Point){ .X = xStart + i, .Y = yStart };
	}

	GenerateApple();
	DrawSnake();
	DrawApple();
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

	if (IsEqual(head, apple))
	{
		score++;
		snakeSize++;
		snakeBody[snakeSize - 1] = head;

		DrawPoint(snakeBody[snakeSize - 2], LCD_COLOR_ORANGE, LCD_COLOR_DARKYELLOW);
		DrawPoint(snakeBody[snakeSize - 1], LCD_COLOR_BLUE, LCD_COLOR_DARKYELLOW);

		if (snakeSize == BUFFER_SIZE)
		{
			return Winning;
		}

		GenerateApple();
		DrawApple();
	}
	else
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

		DrawPoint(tail, LCD_COLOR_WHITE, LCD_COLOR_WHITE);
		DrawPoint(snakeBody[snakeSize - 2], LCD_COLOR_ORANGE, LCD_COLOR_DARKYELLOW);
		DrawPoint(snakeBody[snakeSize - 1], LCD_COLOR_BLUE, LCD_COLOR_DARKYELLOW);
	}

	return None;
}

static void DrawSnake()
{
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	for (size_t i = 0; i < snakeSize - 1; i++)
	{
		DrawPoint(snakeBody[i], LCD_COLOR_ORANGE, LCD_COLOR_DARKYELLOW);
	}

	DrawPoint(snakeBody[snakeSize - 1], LCD_COLOR_BLUE, LCD_COLOR_DARKYELLOW);
}

static void DrawApple()
{
	DrawPoint(apple, LCD_COLOR_RED, LCD_COLOR_RED);
}

static void DrawPoint(Point point, uint16_t innerColor, uint16_t borderColor)
{
	BSP_LCD_SetTextColor(innerColor);
	BSP_LCD_FillRect(point.X * pointSize, point.Y * pointSize, pointSize - 1, pointSize - 1);

	BSP_LCD_SetTextColor(borderColor);
	BSP_LCD_DrawHLine(point.X * pointSize, point.Y * pointSize, pointSize);
	BSP_LCD_DrawHLine(point.X * pointSize, point.Y * pointSize + pointSize - 1, pointSize);

	BSP_LCD_DrawVLine(point.X * pointSize, point.Y * pointSize, pointSize);
	BSP_LCD_DrawVLine(point.X * pointSize + pointSize - 1, point.Y * pointSize, pointSize);
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

static void GenerateApple()
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

static bool IsEqual(Point point1, Point point2)
{
	return point1.X == point2.X && point1.Y == point2.Y;
}

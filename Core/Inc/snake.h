#pragma once
#include "main.h"

typedef enum
{
	Up,
	Down,
	Left,
	Right
}Direction;

typedef enum
{
	Winning,
	Defeat,
	None
}GameState;

void SnakeInit();
GameState Move(Direction direction);
void DrawSnake();
void DrawApple();
void DrawPoint(Point point, uint16_t color);
void GameOver();
void Win();
void GenerateApple();
int8_t IsEqual(Point point1, Point point2);


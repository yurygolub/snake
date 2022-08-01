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
void GameOver();
void Win();


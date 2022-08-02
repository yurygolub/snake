#pragma once
#include "main.h"

typedef enum
{
	Up,
	Down,
	Left,
	Right
}Direction_e;

typedef enum
{
	Winning,
	Defeat,
	None
}GameState_e;

void Setup(uint8_t pSize, uint8_t applesNumToWin);
void SnakeInit();
GameState_e Move(Direction_e direction);
void GameOver();
void Win();


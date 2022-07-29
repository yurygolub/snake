#include "main.h"

struct Point
{
	int16_t x;
	int16_t y;
};

enum Direction
{
	Up,
	Down,
	Left,
	Right
};

enum GameState
{
	Winning,
	Defeat,
	None
};

void SnakeInit();
enum GameState Move(enum Direction direction);
void DrawSnake();
void DrawApple();
void DrawPoint(struct Point point, uint16_t color);
void GameOver();
void Win();
void GenerateApple();
int8_t IsEqual(struct Point point1, struct Point point2);


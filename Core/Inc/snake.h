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

void SnakeInit();
int8_t Move(enum Direction direction);
void DrawSnake();
void DrawApple();
void DrawPoint(struct Point point, uint16_t color);
void GameOver();
void GenerateApple();
int8_t IsEqual(struct Point point1, struct Point point2);


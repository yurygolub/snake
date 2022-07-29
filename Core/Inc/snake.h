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
void Move(enum Direction direction);
void DrawSnake();
void DrawPoint(struct Point point, uint16_t color);




#pragma once
#include "main.h"
#include "snake.h"
#include "life.h"

typedef struct
{
	void (*handler)();
	char* name;
}MenuItem;

void DisplayMenu();

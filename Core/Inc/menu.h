#pragma once
#include "main.h"
#include "life.h"

typedef struct
{
	void (*func)();
	char* name;
}MenuItem;

void DisplayMenu();

#pragma once
#include "main.h"

typedef struct
{
	void (*func)();
	char* name;
}MenuItem;

void DisplayMenuItems();
void ChooseMenuItem();

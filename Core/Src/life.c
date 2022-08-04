#include "life.h"

#define FIELD_SIZE 40

extern RNG_HandleTypeDef hrng;

static bool field[FIELD_SIZE][FIELD_SIZE];
static uint8_t pointSize;
static uint8_t density = 5;

static void LifeInit();
static void NextGeneration();
static uint8_t CountNeighbours(uint16_t xPos, uint16_t yPos);
static void Render();
static void Redraw();
static void DrawPoint(uint16_t xPos, uint16_t yPos, uint16_t color);

void LifeGame()
{
	LifeInit();
	Render();
	HAL_Delay(200);
	while (!HAL_GPIO_ReadPin(JOY_SEL_GPIO_Port, JOY_SEL_Pin))
	{
		NextGeneration();
		Redraw();
	}

	HAL_Delay(100);
}

static void LifeInit()
{
	pointSize = BSP_LCD_GetXSize() / FIELD_SIZE;
	uint32_t randomNumber;

    for (size_t i = 0; i < FIELD_SIZE; i++)
    {
        for (size_t j = 0; j < FIELD_SIZE; j++)
        {
    		HAL_RNG_GenerateRandomNumber(&hrng, &randomNumber);
    		field[i][j] = randomNumber % density ? false : true;
        }
    }
}

static void NextGeneration()
{
	bool newField[FIELD_SIZE][FIELD_SIZE] = { false };
	for (size_t i = 0; i < FIELD_SIZE; i++)
	{
		for (size_t j = 0; j < FIELD_SIZE; j++)
		{
			uint8_t countOfNeighbours = CountNeighbours(i, j);
			if (!field[i][j] && countOfNeighbours == 3)
			{
				newField[i][j] = true;
			}
			else if (field[i][j] && (countOfNeighbours < 2 || countOfNeighbours > 3))
			{
				newField[i][j] = false;
			}
			else
			{
				newField[i][j] = field[i][j];
			}
		}
	}


    for (size_t i = 0; i < FIELD_SIZE; i++)
    {
        for (size_t j = 0; j < FIELD_SIZE; j++)
        {
        	field[i][j] = newField[i][j];
        }
    }
}

static uint8_t CountNeighbours(uint16_t xPos, uint16_t yPos)
{
	int8_t count = 0;
	for (int8_t i = -1; i < 2; i++)
	{
		for (int8_t j = -1; j < 2; j++)
		{
			uint16_t x = (xPos + i + FIELD_SIZE) % FIELD_SIZE;
			uint16_t y = (yPos + j + FIELD_SIZE) % FIELD_SIZE;
            bool isSelfChecking = xPos == x && yPos == y;
            if (field[x][y] && !isSelfChecking)
            {
                count++;
            }
		}
	}

	return count;
}

static void Render()
{
	BSP_LCD_Clear(LCD_COLOR_DARKGRAY);
	for (size_t i = 0; i < FIELD_SIZE; i++)
	{
		for (size_t j = 0; j < FIELD_SIZE; j++)
		{
			if (field[i][j])
			{
				DrawPoint(i, j, LCD_COLOR_RED);
			}
		}
	}
}

static void Redraw()
{
	for (size_t i = 0; i < FIELD_SIZE; i++)
	{
		for (size_t j = 0; j < FIELD_SIZE; j++)
		{
			if (field[i][j])
			{
				DrawPoint(i, j, LCD_COLOR_RED);
			}
			else
			{
				DrawPoint(i, j, LCD_COLOR_DARKGRAY);
			}
		}
	}
}

static void DrawPoint(uint16_t xPos, uint16_t yPos, uint16_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillRect(xPos * pointSize, yPos * pointSize, pointSize - 1, pointSize - 1);
}

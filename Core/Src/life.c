#include "life.h"

#define FIELD_SIZE 40

extern RNG_HandleTypeDef hrng;
extern TIM_HandleTypeDef htim7;

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
		char text[32];
		uint16_t count = (uint16_t)htim7.Instance->CNT;
		NextGeneration();
		uint16_t elapsed = (uint16_t)htim7.Instance->CNT - count;
		snprintf(text, 32, "%.2f ms", elapsed*0.01f);
		Redraw(text);
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

static void Redraw(const char* text)
{
	for (size_t i = 0; i < FIELD_SIZE; i++)
	{
		for (size_t j = 0; j < FIELD_SIZE; j++)
		{
			if (i < 24 && j < 4)
			{
				continue;
			}

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

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAtLine(0, (uint8_t*)text);
}

static void DrawPoint(uint16_t xPos, uint16_t yPos, uint16_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillRect(xPos * pointSize, yPos * pointSize, pointSize - 1, pointSize - 1);
}

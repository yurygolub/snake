#include "life.h"
#include "string.h"

#define LOG_ENABLE
#define FIELD_SIZE 80

extern RNG_HandleTypeDef hrng;
extern TIM_HandleTypeDef htim7;

#define TYPE_SIZE 8
#define ARRAY_SIZE (FIELD_SIZE * FIELD_SIZE / TYPE_SIZE)

static uint8_t field[ARRAY_SIZE];
static uint8_t newField[ARRAY_SIZE];
static uint8_t pointSize;
static uint8_t density = 5;

static void LifeInit();
static void NextGeneration();
static void Render();

#ifndef LOG_ENABLE
static void Redraw();
#endif

#ifdef LOG_ENABLE
static void Redraw(const char* text);
#endif

static void DrawPoint(uint8_t xPos, uint8_t yPos, uint16_t color);

void LifeGame()
{
	LifeInit();
	Render();
	HAL_Delay(200);
	while (!HAL_GPIO_ReadPin(JOY_SEL_GPIO_Port, JOY_SEL_Pin))
	{
#ifndef LOG_ENABLE
		NextGeneration();
		Redraw();
#endif

#ifdef LOG_ENABLE
		char text[32];
		uint16_t count = (uint16_t)htim7.Instance->CNT;
		NextGeneration();
		uint16_t elapsed = (uint16_t)htim7.Instance->CNT - count;
		snprintf(text, 32, "%.2f ms", elapsed*0.01f);
		Redraw(text);
#endif
	}

	HAL_Delay(100);
}

static void LifeInit()
{
	pointSize = BSP_LCD_GetXSize() / FIELD_SIZE;
	uint32_t randomNumber;

    for (size_t i = 0; i < ARRAY_SIZE; i++)
    {
    	for (size_t j = 0; j < TYPE_SIZE; j++)
    	{
    		HAL_RNG_GenerateRandomNumber(&hrng, &randomNumber);
    		field[i] ^= randomNumber % density ? 0 : (1 << j);
		}
    }
}

static void NextGeneration()
{
	for (uint16_t i = 0; i < FIELD_SIZE * FIELD_SIZE; i++)
	{
		uint8_t countOfNeighbours = 0;
		uint16_t index = (i - FIELD_SIZE - 1 + FIELD_SIZE * FIELD_SIZE) % (FIELD_SIZE * FIELD_SIZE);
		countOfNeighbours += (field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0;

		index = (i - FIELD_SIZE + FIELD_SIZE * FIELD_SIZE) % (FIELD_SIZE * FIELD_SIZE);
		countOfNeighbours += (field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0;

		index = (i - FIELD_SIZE + 1 + FIELD_SIZE * FIELD_SIZE) % (FIELD_SIZE * FIELD_SIZE);
		countOfNeighbours += (field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0;

		index = (i - 1 + FIELD_SIZE * FIELD_SIZE) % (FIELD_SIZE * FIELD_SIZE);
		countOfNeighbours += (field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0;

		index = (i + 1 + FIELD_SIZE * FIELD_SIZE) % (FIELD_SIZE * FIELD_SIZE);
		countOfNeighbours += (field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0;

		index = (i + FIELD_SIZE - 1 + FIELD_SIZE * FIELD_SIZE) % (FIELD_SIZE * FIELD_SIZE);
		countOfNeighbours += (field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0;

		index = (i + FIELD_SIZE + FIELD_SIZE * FIELD_SIZE) % (FIELD_SIZE * FIELD_SIZE);
		countOfNeighbours += (field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0;

		index = (i + FIELD_SIZE + 1 + FIELD_SIZE * FIELD_SIZE) % (FIELD_SIZE * FIELD_SIZE);
		countOfNeighbours += (field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0;

		if (countOfNeighbours == 3)
		{
			newField[i / TYPE_SIZE] |= 1 << (i % TYPE_SIZE);
		}
		else if (countOfNeighbours < 2 || countOfNeighbours > 3)
		{
			newField[i / TYPE_SIZE] &= ~(1 << (i % TYPE_SIZE));
		}
		else
		{
			newField[i / TYPE_SIZE] |= field[i / TYPE_SIZE] & (1 << (i % TYPE_SIZE));
		}
	}

	memcpy(field, newField, ARRAY_SIZE);
}

static void Render()
{
	BSP_LCD_Clear(LCD_COLOR_DARKGRAY);
	for (uint16_t i = 0; i < FIELD_SIZE * FIELD_SIZE; i++)
	{
		if (field[i / TYPE_SIZE] & (1 << (i % TYPE_SIZE)))
		{
			DrawPoint(i % FIELD_SIZE, i / FIELD_SIZE, LCD_COLOR_RED);
		}
	}
}

#ifndef LOG_ENABLE
static void Redraw()
#endif

#ifdef LOG_ENABLE
static void Redraw(const char* text)
#endif
{
#ifdef LOG_ENABLE
	uint8_t textWidth = Font16.Width * strlen(text) / pointSize + 1;
	uint8_t textHeight = Font16.Height / pointSize + 1;
#endif

	uint8_t x, y;
	for (uint16_t i = 0; i < FIELD_SIZE * FIELD_SIZE; i++)
	{
		x = i % FIELD_SIZE, y = i / FIELD_SIZE;
#ifdef LOG_ENABLE
		if (x < textWidth && y < textHeight)
		{
			continue;
		}
#endif

		DrawPoint(x, y, field[i / TYPE_SIZE] & (1 << (i % TYPE_SIZE)) ? LCD_COLOR_RED : LCD_COLOR_DARKGRAY);
	}

#ifdef LOG_ENABLE
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)text, LEFT_MODE);
#endif
}

static void DrawPoint(uint8_t xPos, uint8_t yPos, uint16_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillRect(xPos * pointSize, yPos * pointSize, pointSize, pointSize);
}

#include "life.h"
#include "string.h"
#include "magic.h"

#define LOG_ENABLE
#define FIELD_SIZE 40
#define ADDED_FIELD_SIZE (FIELD_SIZE + 2)

extern RNG_HandleTypeDef hrng;
extern TIM_HandleTypeDef htim7;

#define TYPE_SIZE 8
#define ARRAY_SIZE (ADDED_FIELD_SIZE * ADDED_FIELD_SIZE / TYPE_SIZE + 1)

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

    for (size_t i = ADDED_FIELD_SIZE + 1; i < ADDED_FIELD_SIZE * ADDED_FIELD_SIZE - ADDED_FIELD_SIZE - 1; i++)
    {
    	if (i % ADDED_FIELD_SIZE == 0 || i % ADDED_FIELD_SIZE == ADDED_FIELD_SIZE - 1)
    	{
			continue;
		}

		HAL_RNG_GenerateRandomNumber(&hrng, &randomNumber);
		field[i / TYPE_SIZE] ^= randomNumber % density ? 0 : (1 << (i % TYPE_SIZE));
    }
}

static void NextGeneration()
{
	uint16_t buffer = 0, index;
	for (uint16_t i = ADDED_FIELD_SIZE + 1; i < ADDED_FIELD_SIZE * ADDED_FIELD_SIZE - ADDED_FIELD_SIZE - 1; i++)
	{
    	if (i % ADDED_FIELD_SIZE == 0 || i % ADDED_FIELD_SIZE == ADDED_FIELD_SIZE - 1)
    	{
			continue;
		}

    	if (i % ADDED_FIELD_SIZE == 1)
		{
			index = i - ADDED_FIELD_SIZE - 1;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0);

			index = i - 1;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0) << 1;

			index = i + ADDED_FIELD_SIZE - 1;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0) << 2;

			index = i - ADDED_FIELD_SIZE;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0) << 3;

			buffer |= ((field[i / TYPE_SIZE] & (1 << (i % TYPE_SIZE))) != 0) << 4;

			index = i + ADDED_FIELD_SIZE;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0) << 5;

			index = i - ADDED_FIELD_SIZE + 1;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0) << 6;

			index = i + 1;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0) << 7;

			index = i + ADDED_FIELD_SIZE + 1;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0) << 8;
		}
		else
		{
			buffer >>= 3;

			index = i - ADDED_FIELD_SIZE + 1;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0) << 6;

			index = i + 1;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0) << 7;

			index = i + ADDED_FIELD_SIZE + 1;
			buffer |= ((field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))) != 0) << 8;
		}

		uint8_t result = (magicArray[buffer / 8] & (1 << (buffer % 8))) != 0;

		newField[i / TYPE_SIZE] = (newField[i / TYPE_SIZE] & ~(1 << (i % TYPE_SIZE))) |
				(result << (i % TYPE_SIZE));
	}

	memcpy(field, newField, ARRAY_SIZE);
}

static void Render()
{
	BSP_LCD_Clear(LCD_COLOR_DARKGRAY);
	for (uint16_t i = ADDED_FIELD_SIZE + 1; i < ADDED_FIELD_SIZE * ADDED_FIELD_SIZE - ADDED_FIELD_SIZE - 1; i++)
	{
    	if (i % ADDED_FIELD_SIZE == 0 || i % ADDED_FIELD_SIZE == ADDED_FIELD_SIZE - 1)
    	{
			continue;
		}

    	uint16_t index = i - (ADDED_FIELD_SIZE + 1) - 2 * (i / ADDED_FIELD_SIZE - 1);
		if (field[i / TYPE_SIZE] & (1 << (i % TYPE_SIZE)))
		{
			DrawPoint(index % FIELD_SIZE, index / FIELD_SIZE, LCD_COLOR_RED);
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
	for (uint16_t i = ADDED_FIELD_SIZE + 1; i < ADDED_FIELD_SIZE * ADDED_FIELD_SIZE - ADDED_FIELD_SIZE - 1; i++)
	{
    	if (i % ADDED_FIELD_SIZE == 0 || i % ADDED_FIELD_SIZE == ADDED_FIELD_SIZE - 1)
    	{
			continue;
		}

    	uint16_t index = i - (ADDED_FIELD_SIZE + 1) - 2 * (i / ADDED_FIELD_SIZE - 1);
		x = index % FIELD_SIZE, y = index / FIELD_SIZE;
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

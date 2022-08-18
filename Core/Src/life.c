#include "life.h"
#include "string.h"
#include "magic.h"
#include "bit_banding.h"

#define LOG_ENABLE
#define FIELD_SIZE 40
#define ADDED_FIELD_SIZE (FIELD_SIZE + 2)

extern RNG_HandleTypeDef hrng;
extern TIM_HandleTypeDef htim7;

#define TYPE_SIZE 8
#define ARRAY_SIZE (ADDED_FIELD_SIZE * ADDED_FIELD_SIZE / TYPE_SIZE + 1)

static uint8_t field[ARRAY_SIZE] = { 0 };
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

    for (uint16_t i = 1; i < ADDED_FIELD_SIZE - 1; i++)
    {
		for (uint16_t j = 1; j < ADDED_FIELD_SIZE - 1; j++)
		{
	    	uint16_t index = i * ADDED_FIELD_SIZE + j;
			HAL_RNG_GenerateRandomNumber(&hrng, &randomNumber);
			field[index / TYPE_SIZE] ^= randomNumber % density ? 0 : (1 << (index % TYPE_SIZE));
		}
    }
}

static void NextGeneration()
{
	for (uint16_t i = 1; i < ADDED_FIELD_SIZE - 1; i++)
	{
		#define READ_FIELD_CELL(index) (BIT_BAND_SRAM(&field[(index) / TYPE_SIZE], (index) % TYPE_SIZE))
		uint16_t buffer = 0;
		uint16_t index = (i - 1) * ADDED_FIELD_SIZE;
		buffer |= READ_FIELD_CELL(index);

		index = i * ADDED_FIELD_SIZE;
		buffer |= READ_FIELD_CELL(index) << 1;

		index = (i + 1) * ADDED_FIELD_SIZE;
		buffer |= READ_FIELD_CELL(index) << 2;

		index = (i - 1) * ADDED_FIELD_SIZE + 1;
		buffer |= READ_FIELD_CELL(index) << 3;

		index = i * ADDED_FIELD_SIZE + 1;
		buffer |= READ_FIELD_CELL(index) << 4;

		index = (i + 1) * ADDED_FIELD_SIZE + 1;
		buffer |= READ_FIELD_CELL(index) << 5;

		for (uint16_t j = 1; j < ADDED_FIELD_SIZE - 1; j++)
		{
			index = (i - 1) * ADDED_FIELD_SIZE + j + 1;
			buffer |= READ_FIELD_CELL(index) << 6;

			index = i * ADDED_FIELD_SIZE + j + 1;
			buffer |= READ_FIELD_CELL(index) << 7;

			index = (i + 1) * ADDED_FIELD_SIZE + j + 1;
			buffer |= READ_FIELD_CELL(index) << 8;

			index = i * ADDED_FIELD_SIZE + j;
			uint8_t result = (magicArray[buffer / 8] & (1 << (buffer % 8))) != 0;

			BIT_BAND_SRAM(&newField[index / TYPE_SIZE], index % TYPE_SIZE) = result;

			buffer >>= 3;
		}
        #undef READ_FIELD_CELL
	}

	memcpy(field, newField, ARRAY_SIZE);
}

static void Render()
{
	BSP_LCD_Clear(LCD_COLOR_DARKGRAY);
	for (uint16_t i = 1; i < ADDED_FIELD_SIZE - 1; i++)
	{
		for (uint16_t j = 1; j < ADDED_FIELD_SIZE - 1; j++)
		{
	    	uint16_t index = i * ADDED_FIELD_SIZE + j;
			if (field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE)))
			{
				DrawPoint(j - 1, i - 1, LCD_COLOR_RED);
			}
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

	for (uint16_t i = 1; i < ADDED_FIELD_SIZE - 1; i++)
	{
		for (uint16_t j = 1; j < ADDED_FIELD_SIZE - 1; j++)
		{
			#ifdef LOG_ENABLE
			if (i - 1 < textHeight && j - 1 < textWidth)
			{
				continue;
			}
			#endif

			uint16_t index = i * ADDED_FIELD_SIZE + j;
			DrawPoint(j - 1, i - 1, field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE)) ? LCD_COLOR_RED : LCD_COLOR_DARKGRAY);
		}
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
	#if FIELD_SIZE == 240
	BSP_LCD_DrawPixel(xPos, yPos, color);
	#else
	BSP_LCD_FillRect(xPos * pointSize, yPos * pointSize, pointSize, pointSize);
	#endif
}

#include "life.h"
#include "string.h"
#include "magic.h"
#include "bit_banding.h"
#include "st7789h2/st7789h2.h"

//#define LOG_ENABLE
#define LOG_TO_UART

#define FIELD_SIZE 240
#define ADDED_FIELD_SIZE (FIELD_SIZE + 2)
#define DENSITY 5

extern RNG_HandleTypeDef hrng;

#if defined (LOG_ENABLE) || defined (LOG_TO_UART)
extern TIM_HandleTypeDef htim7;
#endif

#ifdef LOG_TO_UART
extern UART_HandleTypeDef huart2;
#endif

#define TYPE_SIZE 8
#define ARRAY_SIZE (ADDED_FIELD_SIZE * ADDED_FIELD_SIZE / TYPE_SIZE + 1)

static uint8_t field[ARRAY_SIZE] = { 0 };
static uint8_t newField[ARRAY_SIZE];
static uint8_t pointSize;

static void LifeInit();
static void NextGeneration();

#ifndef LOG_ENABLE
static void Render();
#endif

#ifdef LOG_ENABLE
static void Render(const char* text);
#endif

void LifeGame()
{
	LifeInit();

	HAL_Delay(200);

	#if defined(LOG_ENABLE) || defined(LOG_TO_UART)
	char text[16] = { '\0' };
	#endif

	while (!HAL_GPIO_ReadPin(JOY_SEL_GPIO_Port, JOY_SEL_Pin))
	{
		#ifndef LOG_ENABLE
		#ifndef LOG_TO_UART
		Render();
		NextGeneration();
		#endif
		#endif

		#ifndef LOG_ENABLE
		#ifdef LOG_TO_UART
		uint16_t count = (uint16_t)htim7.Instance->CNT;
		Render();
		NextGeneration();
		uint16_t elapsed = (uint16_t)htim7.Instance->CNT - count;
		snprintf(text, 16, "%.2f ms\n\r", elapsed * 0.01f);
		HAL_UART_Transmit(&huart2, (const uint8_t*)text, sizeof(text), 1);
		#endif
		#endif

		#ifndef LOG_TO_UART
		#ifdef LOG_ENABLE
		uint16_t count = (uint16_t)htim7.Instance->CNT;
		Render(text);
		NextGeneration();
		uint16_t elapsed = (uint16_t)htim7.Instance->CNT - count;
		snprintf(text, 16, "%.2f ms", elapsed * 0.01f);
		#endif
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
			field[index / TYPE_SIZE] ^= randomNumber % DENSITY ? 0 : (1 << (index % TYPE_SIZE));
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

#ifdef LOG_ENABLE
#if FIELD_SIZE == 240
static void Render(const char* text)
{
	uint8_t textWidth = Font16.Width * strlen(text) / pointSize + 1;
	uint8_t textHeight = Font16.Height / pointSize + 1;

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)text, LEFT_MODE);

	ST7789H2_SetCursor(0, 0);
	LCD_IO_WriteReg(ST7789H2_WRITE_RAM);

	for (uint16_t i = 1; i < ADDED_FIELD_SIZE - 1; i++)
	{
		if (i - 1 < textHeight)
		{
			ST7789H2_SetCursor(textWidth * pointSize, (i - 1) * pointSize);
			LCD_IO_WriteReg(ST7789H2_WRITE_RAM);
		}
		else
		{
			ST7789H2_SetCursor(0, (i - 1) * pointSize);
			LCD_IO_WriteReg(ST7789H2_WRITE_RAM);
		}

		for (uint16_t j = i - 1 < textHeight ? textWidth + 1 : 1;
				j < ADDED_FIELD_SIZE - 1; j++)
		{
			uint16_t index = i * ADDED_FIELD_SIZE + j;
			LCD_IO_WriteData(field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))
					? LCD_COLOR_RED : LCD_COLOR_DARKGRAY);
		}
	}
}
#else
static void Render(const char* text)
{
	uint8_t textWidth = Font16.Width * strlen(text) / pointSize + 1;
	uint8_t textHeight = Font16.Height / pointSize + 1;

	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)text, LEFT_MODE);

	for (uint16_t i = 1; i < ADDED_FIELD_SIZE - 1; i++)
	{
		for (uint8_t k = 0;  k < pointSize; k++)
		{
			if (i - 1 < textHeight)
			{
				ST7789H2_SetCursor(textWidth * pointSize, (i - 1) * pointSize + k);
				LCD_IO_WriteReg(ST7789H2_WRITE_RAM);
			}
			else
			{
				ST7789H2_SetCursor(0, (i - 1) * pointSize + k);
				LCD_IO_WriteReg(ST7789H2_WRITE_RAM);
			}

			for (uint16_t j = i - 1 < textHeight ? textWidth + 1 : 1;
					j < ADDED_FIELD_SIZE - 1; j++)
			{
				for (uint8_t m = 0;  m < pointSize; m++)
				{
					uint16_t index = i * ADDED_FIELD_SIZE + j;
					LCD_IO_WriteData(field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))
							? LCD_COLOR_RED : LCD_COLOR_DARKGRAY);
				}
			}
		}
	}
}
#endif // FIELD_SIZE == 240
#endif // LOG_ENABLE

#ifndef LOG_ENABLE
#if FIELD_SIZE == 240
static void Render()
{
	ST7789H2_SetCursor(0, 0);
	LCD_IO_WriteReg(ST7789H2_WRITE_RAM);

	for (uint16_t i = 1; i < ADDED_FIELD_SIZE - 1; i++)
	{
		for (uint16_t j = 1; j < ADDED_FIELD_SIZE - 1; j++)
		{
			uint16_t index = i * ADDED_FIELD_SIZE + j;
			LCD_IO_WriteData(field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))
					? LCD_COLOR_RED : LCD_COLOR_DARKGRAY);
		}
	}
}
#else
static void Render()
{
	ST7789H2_SetCursor(0, 0);
	LCD_IO_WriteReg(ST7789H2_WRITE_RAM);

	for (uint16_t i = 1; i < ADDED_FIELD_SIZE - 1; i++)
	{
		for (uint8_t k = 0;  k < pointSize; k++)
		{
			for (uint16_t j = 1; j < ADDED_FIELD_SIZE - 1; j++)
			{
				for (uint8_t m = 0;  m < pointSize; m++)
				{
					uint16_t index = i * ADDED_FIELD_SIZE + j;
					LCD_IO_WriteData(field[index / TYPE_SIZE] & (1 << (index % TYPE_SIZE))
							? LCD_COLOR_RED : LCD_COLOR_DARKGRAY);
				}
			}
		}
	}
}
#endif // FIELD_SIZE == 240
#endif // LOG_ENABLE

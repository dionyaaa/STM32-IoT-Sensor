#include "stm32f4xx_hal.h"

#include <assert.h> // Для assert().
#include <stdint.h> // Для типа uint8_t.

#include "config.h"
#include "board.h"
#include "display.h"

// Статическая функция установки четырёх бит на шину (DB4-DB7):
static void display_set_data_pins(uint8_t nibble)
{
    HAL_GPIO_WritePin(DISPLAY_GPIO_PORT, DISPLAY_DB4_PIN,
                    (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISPLAY_GPIO_PORT, DISPLAY_DB5_PIN,
                    (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISPLAY_GPIO_PORT, DISPLAY_DB6_PIN,
                    (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISPLAY_GPIO_PORT, DISPLAY_DB7_PIN,
                    (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);             
}

// Статическая функция генерации импульса на пине E:
static void display_pulse_enable(void)
{
    HAL_GPIO_WritePin(DISPLAY_GPIO_PORT, DISPLAY_E_PIN, GPIO_PIN_SET);
    Delay_us(2);
    HAL_GPIO_WritePin(DISPLAY_GPIO_PORT, DISPLAY_E_PIN, GPIO_PIN_RESET);
    Delay_us(50);
}

// Статическая функция отправки полубайта на шину:
static void display_send_4_bits(uint8_t data)
{
    display_set_data_pins(data);
    display_pulse_enable();
}

// Статическая функция отправки полного байта в 4-битном режиме, GPIO_PIN_RESET - команда, GPIO_PIN_SET - данные:
static void display_send(uint8_t value, uint8_t mode)
{
    HAL_GPIO_WritePin(DISPLAY_GPIO_PORT, DISPLAY_RS_PIN, mode);
    display_send_4_bits((value >> 4) & 0x0F);
    display_send_4_bits(value & 0x0F);
}

// Функция инициализации дисплея в 4-битном режиме:
void Display_Init(void)
{
    HAL_Delay(50);

    display_send_4_bits(0x03);
    HAL_Delay(5);
    display_send_4_bits(0x03);
    Delay_us(150);
    display_send_4_bits(0x03);
    Delay_us(150);

    display_send_4_bits(0x02);
    Delay_us(150);

    display_send(DISPLAY_CMD_4BIT_2LINE_5x8, GPIO_PIN_RESET);
    Delay_us(50);
    display_send(DISPLAY_CMD_DISPLAY_OFF, GPIO_PIN_RESET);
    Delay_us(50);
    display_send(DISPLAY_CLEAR, GPIO_PIN_RESET);
    HAL_Delay(2);
    display_send(DISPLAY_CMD_ENTRY_RIGHT, GPIO_PIN_RESET);
    Delay_us(50);
    display_send(DISPLAY_CMD_DISPLAY_ON, GPIO_PIN_RESET);
    Delay_us(50);
}

// Функция очистки дисплея:
void Display_Clear(void)
{
    display_send(DISPLAY_CLEAR, GPIO_PIN_RESET);
    HAL_Delay(2);
}

// Функция вывода строки на дисплей:
void Display_PrintString(const char* str)
{
    // Проверка передаваемых данных:
    assert(str != NULL);

    // Посимвольный вывод строки:
    while(*str != '\0')
    {
        display_send((uint8_t)*str, GPIO_PIN_SET);
        str++;
        Delay_us(50);
    }
}
#include "stm32f4xx_hal.h"

#include <assert.h> // Для assert().
#include <stdio.h> // Для sprintf().
#include <stdint.h> // Для типов uint8_t и uint16_t.
#include <string.h> // Для strlen().

#include "test.h"

// Инициализация статических переменных:
static UART_HandleTypeDef* huart_test = NULL;
static I2C_HandleTypeDef* hi2c_test = NULL;
static char buffer[128];

// Статическая функция для вывода текста с помощью UART:
static void test_uart_send(const char* message)
{
    HAL_UART_Transmit(huart_test, (uint8_t*)message, strlen(message), 100);
}

// Функция объявления переменных тестируемой периферии:
void Test_Init(UART_HandleTypeDef* huart, I2C_HandleTypeDef* hi2c)
{
    assert(huart != NULL && hi2c != NULL);
    huart_test = huart;
    hi2c_test = hi2c;
    test_uart_send("Test module initialized\r\n");
}

// Функция теста отправки сообщения по UART:
void Test_UART_Send(const char* message)
{
    assert(message != NULL && huart_test != NULL);
    test_uart_send(message);
}

// Функция сканирования подключённых по I2C устройств:
void Test_I2C_Scan(void)
{
    assert(hi2c_test != NULL);
    test_uart_send("Scanning I2C...\r\n");
    uint8_t found_devices = 0;

    for (uint16_t addr = 1; addr < 128; addr++)
    {
        if (HAL_I2C_IsDeviceReady(hi2c_test, (addr << 1), 2, 10) == HAL_OK)
        {
            sprintf(buffer, "Found at 0x%02X\r\n", addr);
            test_uart_send(buffer);
            found_devices++;
        }
    }

    if(found_devices == 0)
    {
        test_uart_send("No I2C devices found\r\n");
    }
}

// Функция чтения регистров подключённых устройств:
void Test_I2C_ReadReg(uint16_t dev_addr, uint16_t reg_addr)
{
    assert(hi2c_test != NULL);
    uint8_t value = 0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c_test, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);

    if (status == HAL_OK)
    {
        sprintf(buffer, "Reg_addr 0x%02X at 0x%02X = 0x%02X\r\n", reg_addr, dev_addr, value);
    }
    else
    {
        sprintf(buffer, "Read error: %d (Dev_addr: 0x%02X, reg_addr: 0x%02X)\r\n", status, dev_addr, reg_addr);
    }
    test_uart_send(buffer);
}
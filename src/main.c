#include "stm32f4xx_hal.h"

#include <stdio.h> // Для sprintf().
#include <string.h> // Для strlen().

#include "board.h"
#include "sensors.h"

int main(void)
{
    // Инициализация HAL:
    HAL_Init();

    // Настройка тактирования:
    SystemClock_Config();

    // Инициализация периферии:
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_I2C1_Init();

    // Инициализация периферии успешна, горит только красный светодиод:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

    // Инициализация драйверов периферии:
    Sensors_Init();

    // Инициализация драйверов периферии успешна, горит только зелёный светодиод:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

    // Переменные:
    char buffer[128];
    SensorsData_t data;

    while (1)
    {
        // Главный цикл.
        Sensors_ReadAll(&data); // Чтение всех показаний датчика.
        HAL_Delay(1000); // Задержка перед повторным чтением.
    }
}
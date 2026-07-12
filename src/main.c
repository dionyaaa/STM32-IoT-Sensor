#include "stm32f4xx_hal.h"

#include <stdio.h> // Для sprintf().
#include <string.h> // Для strlen().

#include "board.h"
#include "sensors.h"
#include "network.h"

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
    Sensors_Init(&hi2c1);
    Network_Init(&huart2, &huart3);

    // Подключение к Wi-Fi-сети:
    Network_ConnectWiFi();

    // Инициализация драйверов периферии и подключение к Wi-Fi прошло успешно, горит только зелёный светодиод:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

    // Переменные:
    SensorsData_t data;

    while (1)
    {
        // Главный цикл.
        Sensors_ReadAll(&data); // Чтение всех показаний датчика.
        Network_Send(&data); // Отправка данных на сервер.
        HAL_Delay(10000); // Задержка перед повторным чтением.
    }
}
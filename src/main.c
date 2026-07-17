#include "stm32f4xx_hal.h"

#include <stdio.h> // Для sprintf().
#include <string.h> // Для strlen().

#include "board.h"
#include "display.h"
#include "sensors.h"
#include "network.h"

int main(void)
{
    // Горит только синий светодиод / ничего не горит - ошибка при инициализации периферии.
    // Горит красный и синий светодиод - ошибка при инициализации драйверов периферийных устройств.
    // Горит зелёный и синий светодиод - ошибка в главном цикле.

    // Инициализация HAL:
    HAL_Init();

    // Настройка тактирования:
    SystemClock_Config();

    // Инициализации счётчика DWT:
    DWT_Init();
    
    // Инициализация периферии:
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_I2C1_Init();

    // Инициализация периферии успешна, горит только красный светодиод:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

    // Инициализация драйверов периферийных устройств:
    Display_Init();
    Sensors_Init(&hi2c1);
    Network_Init(&huart2, &huart3);

    // Инициализация драйверов периферии и подключение к Wi-Fi прошло успешно, горит только зелёный светодиод:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

    // Переменные:
    SensorsData_t data;

    while (1)
    {
        // Главный цикл:
        Sensors_ReadAll(&data); // Чтение всех показаний датчика.

        if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET) // Проверка нажатия кнопки.
        {
            if(Network_CheckWiFi() == NETWORK_OK) // Проверка подключения к Wi-Fi-сети.
            {
                if(Network_Send(&data) == NETWORK_OK) // Попытка отправки данных на сервер.
                {
                    // Сообщение на дисплее об успешной отправке данных на сервер.
                }
                else
                {
                    // Сообщение на дисплее об ошибке при отправке данных на сервер.
                }
            }
            else
            {
                Network_ConnectWiFi(); // Попытка подключения к Wi-Fi-сети.

                if(Network_CheckWiFi() == NETWORK_OK) // Проверка подключения к Wi-Fi-сети.
                {
                    if(Network_Send(&data) == NETWORK_OK) // Попытка отправки данных на сервер.
                    {
                        // Сообщение на дисплее об успешной отправке данных на сервер.
                    }
                    else
                    {
                        // Сообщение на дисплее об ошибке при отправке данных на сервер.
                    }
                }
                else
                {
                    // Сообщение на дисплее об ошибке при подключении к сети.
                }
            }
        }

        HAL_Delay(1000); // Задержка перед повторным чтением.
    }
}
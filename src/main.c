#include "stm32f4xx_hal.h"

#include <stdio.h> // Для sprintf().
#include <string.h> // Для strlen().

#include "config.h"
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
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);

    // Инициализация драйверов периферийных устройств:
    Display_Init();
    Sensors_Init(&hi2c1);
    Network_Init(&huart2, &huart3);

    // Инициализация драйверов периферии и подключение к Wi-Fi прошло успешно, горит только зелёный светодиод:
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);

    // Переменные:
    SensorsData_t data;
    char sensor_readings[21];

    const char* wifi_connection = "Wi-Fi connection... ";
    const char* server_sending = "Sending to server...";
    const char* wifi_error = "    Wi-Fi error!    ";
    const char* sending_error = "   Sending error!   ";
    const char* sending_completed = " Sending completed! ";

    // Главный цикл:
    while (1)
    {
        // Чтение всех показаний датчика:
        Sensors_ReadAll(&data);

        // Вывод показаний датчика на дисплей:
        sprintf(sensor_readings, "Sensor readings:    ");
        Display_SetCursor(0, 0);
        Display_PrintString(sensor_readings);

        sprintf(sensor_readings, "T: %-15.2f C", data.temperature);
        Display_SetCursor(1, 0);
        Display_PrintString(sensor_readings);

        sprintf(sensor_readings, "P: %-13.2f hPa", data.pressure);
        Display_SetCursor(2, 0);
        Display_PrintString(sensor_readings);

        sprintf(sensor_readings, "H: %-15.2f %%", data.humidity);
        Display_SetCursor(3, 0);
        Display_PrintString(sensor_readings);

        // Проверка нажатия кнопки:
        if(HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, BUTTON_PIN) == GPIO_PIN_SET)
        {
            // Ожидание перед очисткой дисплея:
            HAL_Delay(1000);

            // Очистка дисплея и установка курсора в нужное место:
            Display_Clear();
            Display_SetCursor(1, 0);

            // Подключение к Wi-Fi-сети, если это необходимо:
            if(Network_CheckWiFi() != NETWORK_OK)
            {
                Display_PrintString(wifi_connection); // Сообщение о попытке подключения к Wi-Fi-сети.
                Network_ConnectWiFi(); // Попытка подключения к Wi-Fi-сети.
                Display_SetCursor(1, 0);
            }

            // Проверка подключения к Wi-Fi-сети:
            if(Network_CheckWiFi() == NETWORK_OK)
            {
                Display_PrintString(server_sending); // Сообщение о попытке отправки данных на сервер.
                Display_SetCursor(1, 0);
                
                if(Network_Send(&data) == NETWORK_OK) // Попытка отправки данных на сервер.
                {
                    Display_PrintString(sending_completed); // Сообщение об успешной отправке данных на сервер.
                }
                else
                {
                    Display_PrintString(sending_error); // Сообщение об ошибке отправки данных на сервер.
                }
            }
            else
            {
                Display_PrintString(wifi_error); // Сообщение об ошибке подключения к Wi-Fi-сети.
            }

            // Ожидание для прочтения ответов:
            HAL_Delay(2000);
        }

        // Задержка перед повторным чтением:
        HAL_Delay(1000);
    }
}
#include "stm32f4xx_hal.h"

#include <assert.h> // Для assert().
#include <stdio.h> // Для sprintf().
#include <stddef.h> // Для типа size_t.
#include <string.h> // Для strlen(), strstr(), strchr() и memset().

#include "config.h"
#include "board.h"
#include "sensors.h"
#include "network.h"

// Инициализация статических переменных:
static UART_HandleTypeDef* network_huart = NULL;
static UART_HandleTypeDef* console_huart = NULL;
static char response[RESPONSE_SIZE];

// Статическая функция для отправки AT-команды Wi-Fi-модулю:
static uint8_t network_uart_send(const char* command, char* buffer, size_t buffer_size)
{
    // Очистка буфера ответа Wi-Fi-модуля:
    memset(buffer, 0, buffer_size);

    // Отправка команды Wi-Fi-модулю:
    HAL_UART_Transmit(network_huart, (uint8_t*)command, strlen(command), 1000);
    HAL_UART_Transmit(network_huart, (uint8_t*)"\r\n", 2, 1000);
    HAL_UART_Receive(network_huart, (uint8_t*)buffer, buffer_size, 1000);

    // Вывод отправленных команд и полученных ответов в консоль:
    if(console_huart != NULL)
    {
        HAL_UART_Transmit(console_huart, (uint8_t*)"Command:\r\n", 10, 1000);
        HAL_UART_Transmit(console_huart, (uint8_t*)command, strlen(command), 1000);
        HAL_UART_Transmit(console_huart, (uint8_t*)"\r\n\r\n", 4, 1000);
        HAL_UART_Transmit(console_huart, (uint8_t*)"Response:\r\n", 11, 1000);
        HAL_UART_Transmit(console_huart, (uint8_t*)buffer, buffer_size, 1000);
        HAL_UART_Transmit(console_huart, (uint8_t*)"\r\n\r\n", 4, 1000);
    }

    // Проверка ответа Wi-Fi-модуля:
    if(strstr(buffer, "OK") != NULL)
    {
        return NETWORK_OK;
    }
    else
    {
        return NETWORK_ERROR;
    }
}

// Функция инициализации Wi-Fi-модуля:
void Network_Init(UART_HandleTypeDef* huart, UART_HandleTypeDef* debug_huart)
{
    // Проверка передаваемых данных:
    assert(huart != NULL);

    // Подключение UART:
    network_huart = huart;
    if(debug_huart != NULL)
    {
        console_huart = debug_huart;
    }

    // Проверка Wi-Fi-модуля:
    if(network_uart_send("AT", response, RESPONSE_SIZE) != NETWORK_OK)
    {
        Error_Handler();
    }

    // Перезагрузка Wi-Fi-модуля:
    if(network_uart_send("AT+RST", response, RESPONSE_SIZE) != NETWORK_OK)
    {
        Error_Handler();
    }

    HAL_Delay(RESTART_TIME);

    // Установка режима работы Wi-Fi-модуля в режиме Wi-Fi-клиента:
    if(network_uart_send("AT+CWMODE_CUR=1", response, RESPONSE_SIZE) != NETWORK_OK)
    {
        Error_Handler();
    }
}

// Функция подключения к Wi-Fi-сети:
void Network_ConnectWiFi(void)
{
    // Проверка инициализации Wi-Fi-модуля:
    assert(network_huart != NULL);

    // Подключение к Wi-Fi-сети:
    char wifi_connect[128];
    sprintf(wifi_connect, "AT+CWJAP_CUR=\"%s\",\"%s\"", WIFI_SSID, WIFI_PASS);
    network_uart_send(wifi_connect, response, RESPONSE_SIZE);

    HAL_Delay(WIFI_CONNECT_TIMEOUT);
}

// Функция проверки подключения к Wi-Fi-сети:
uint8_t Network_CheckWiFi(void)
{
    // Проверка инициализации Wi-Fi-модуля:
    assert(network_huart != NULL);

    // Проверка подключения к Wi-Fi-сети:
    if(network_uart_send("AT+CWJAP_CUR?", response, RESPONSE_SIZE) == NETWORK_OK && strstr(response, WIFI_SSID) != NULL)
    {
        return NETWORK_OK;
    }
    else
    {
        return NETWORK_ERROR;
    }
}

// Функция отправки данных на сервер:
uint8_t Network_Send(SensorsData_t* data)
{
    // Проверка передаваемых данных:
    assert(data != NULL && network_huart != NULL);

    // Подключение к серверу:
    char server_connect[128];
    sprintf(server_connect, "AT+CIPSTART=\"SSL\",\"%s\",%d", HOST, PORT);
    network_uart_send(server_connect, response, RESPONSE_SIZE);

    HAL_Delay(SERVER_CONNECT_TIMEOUT);

    // Проверка корректности подключения к серверу:
    if(network_uart_send("AT+CIPSTATUS", response, RESPONSE_SIZE) != NETWORK_OK || strstr(response, "STATUS:3") == NULL)
    {
        return NETWORK_ERROR;
    }

    // Создание строки типа JSON:
    char json_data[256];
    sprintf(json_data,
            "{\r\n"
            "\"temperature\": %.2f,\r\n"
            "\"pressure\": %.2f,\r\n"
            "\"humidity\": %.2f\r\n"
            "}",
            data->temperature, data->pressure, data->humidity
    );

    // Создание строки с HTTP-запросом:
    char request[512];
    sprintf(request,
            "POST %s HTTP/1.1\r\n"
            "Host: %s:%d\r\n"
            "X-Sensor-ID: %s\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "\r\n"
            "%s",
            PATH, HOST, PORT, SENSOR_ID, strlen(json_data), json_data
    );

    // Подготовка к отправке данных на сервер:
    sprintf(server_connect, "AT+CIPSEND=%d", strlen(request));
    if(network_uart_send(server_connect, response, RESPONSE_SIZE) != NETWORK_OK || strchr(response, '>') == NULL)
    {
        return NETWORK_ERROR;
    }

    // Отправка данных на сервер:
    memset(response, 0, RESPONSE_SIZE);
    HAL_UART_Transmit(network_huart, (uint8_t*)request, strlen(request), 1000);
    HAL_UART_Receive(network_huart, (uint8_t*)response, RESPONSE_SIZE, 1000);
    if(console_huart != NULL)
    {
        HAL_UART_Transmit(console_huart, (uint8_t*)"Command:\r\n", 10, 1000);
        HAL_UART_Transmit(console_huart, (uint8_t*)request, strlen(request), 1000);
        HAL_UART_Transmit(console_huart, (uint8_t*)"\r\n\r\n", 4, 1000);
        HAL_UART_Transmit(console_huart, (uint8_t*)"Response:\r\n", 11, 1000);
        HAL_UART_Transmit(console_huart, (uint8_t*)response, RESPONSE_SIZE, 1000);
        HAL_UART_Transmit(console_huart, (uint8_t*)"\r\n\r\n", 4, 1000);
    }
    if(strstr(response, "SEND OK") == NULL)
    {
        return NETWORK_ERROR;
    }

    HAL_Delay(SEND_TIME);

    // Закрытие соединения, если не было закрыто со стороны сервера:
    network_uart_send("AT+CIPCLOSE", response, RESPONSE_SIZE);

    return NETWORK_OK;
}
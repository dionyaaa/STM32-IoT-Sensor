#ifndef NETWORK_H
#define NETWORK_H

#include "stm32f4xx_hal.h"
#include "sensors.h"

#define NETWORK_OK 0
#define NETWORK_ERROR 1

#define RESPONSE_SIZE 256

#define RESTART_TIME 2000
#define SEND_TIME 2000

#define WIFI_CONNECT_TIMEOUT 10000
#define SERVER_CONNECT_TIMEOUT 2000

void Network_Init(UART_HandleTypeDef* huart, UART_HandleTypeDef* debug_huart);
void Network_ConnectWiFi(void);
void Network_Send(SensorsData_t* data);

#endif
#ifndef BOARD_H
#define BOARD_H

#include "stm32f4xx_hal.h"
#include <stdint.h> // Для типа uint32_t.

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern I2C_HandleTypeDef hi2c1;

void Error_Handler(void);
void Delay_us(uint32_t us);
void SystemClock_Config(void);
void DWT_Init(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);
void MX_I2C1_Init(void);

#endif
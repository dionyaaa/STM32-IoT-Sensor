#ifndef TEST_H
#define TEST_H

#include "stm32f4xx_hal.h"
#include <stdint.h> // Для типа uint16_t.

void Test_Init(UART_HandleTypeDef* huart, I2C_HandleTypeDef* hi2c);
void Test_UART_Send(const char* message);
void Test_I2C_Scan(void);
void Test_I2C_ReadReg(uint16_t dev_addr, uint16_t reg_addr);

#endif
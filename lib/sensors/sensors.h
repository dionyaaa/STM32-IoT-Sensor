#ifndef SENSORS_H
#define SENSORS_H

#include "stm32f4xx_hal.h"

typedef struct SensorsData_t
{
    double temperature;
    double pressure;
    double humidity;
} SensorsData_t;

void Sensors_Init(I2C_HandleTypeDef* hi2c);
void Sensors_ReadAll(SensorsData_t* data);

#endif
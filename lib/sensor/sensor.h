#ifndef SENSOR_H
#define SENSOR_H

#include "stm32f4xx_hal.h"

typedef struct SensorData_t
{
    double temperature;
    double pressure;
    double humidity;
} SensorData_t;

void Sensor_Init(I2C_HandleTypeDef* hi2c);
void Sensor_ReadAll(SensorData_t* data);

#endif
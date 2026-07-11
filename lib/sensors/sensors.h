#ifndef SENSORS_H
#define SENSORS_H

typedef struct SensorsData_t
{
    double temperature;
    double pressure;
    double humidity;
} SensorsData_t;

void Sensors_Init(void);
void Sensors_ReadAll(SensorsData_t* data);

#endif
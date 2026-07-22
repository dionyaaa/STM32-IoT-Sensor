#include "stm32f4xx_hal.h"
#include "bme280/bme280.h"

#include <assert.h> // Для assert().
#include <stdint.h> // Для типов uint8_t и uint32_t.

#include "board.h"
#include "sensors.h"

// Инициализация статических переменных:
static I2C_HandleTypeDef* sensors_hi2c = NULL;

static uint8_t dev_addr = BME280_I2C_ADDR_PRIM;
static struct bme280_dev dev;
static struct bme280_settings settings;

// Функции-обёртки для официальной библиотеки датчика BME280:
static int8_t bme280_i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t length, void* intf_ptr)
{
    uint8_t intf_addr = *(uint8_t*)intf_ptr;
    int8_t result = HAL_I2C_Mem_Read(sensors_hi2c, intf_addr << 1, reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, length, 250);
    if(result != HAL_OK)
    {
        return BME280_E_COMM_FAIL;
    }
    else
    {
        return BME280_OK;
    }
}

static int8_t bme280_i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t length, void* intf_ptr)
{
    uint8_t intf_addr = *(uint8_t*)intf_ptr;
    int8_t result = HAL_I2C_Mem_Write(sensors_hi2c, intf_addr << 1, reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)reg_data, length, 250);
    if(result != HAL_OK)
    {
        return BME280_E_COMM_FAIL;
    }
    else
    {
        return BME280_OK;
    }
}

static void bme280_delay_us(uint32_t period, void* intf_ptr)
{
    Delay_us(period);
}

// Функция инициализации датчиков:
void Sensors_Init(I2C_HandleTypeDef* hi2c)
{
    // Проверка передаваемых данных:
    assert(hi2c != NULL);

    // Подключение I2C:
    sensors_hi2c = hi2c;

    // Код возврата:
    int8_t result = BME280_OK;

    // Инициализация датчика:
    dev.intf = BME280_I2C_INTF;
    dev.read = bme280_i2c_read;
    dev.write = bme280_i2c_write;
    dev.delay_us = bme280_delay_us;
    dev.intf_ptr = &dev_addr;

    result = bme280_init(&dev);
    if(result != BME280_OK)
    {
        Error_Handler();
    }

    // Настройки режима работы датчика:
    result = bme280_get_sensor_settings(&settings, &dev);
    if(result != BME280_OK)
    {
        Error_Handler();
    }

    settings.filter = BME280_FILTER_COEFF_2;
    settings.osr_h = BME280_OVERSAMPLING_1X;
    settings.osr_p = BME280_OVERSAMPLING_1X;
    settings.osr_t = BME280_OVERSAMPLING_1X;
    settings.standby_time = BME280_STANDBY_TIME_0_5_MS;

    result = bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, &dev);
    if(result != BME280_OK)
    {
        Error_Handler();
    }

    result = bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &dev);
    if(result != BME280_OK)
    {
        Error_Handler();
    }
}

// Функция чтения всех показаний датчика:
void Sensors_ReadAll(SensorsData_t* data)
{
    // Проверка передаваемых данных:
    assert(data != NULL && sensors_hi2c != NULL);

    // Переменные:
    int8_t result = BME280_OK;
    struct bme280_data comp_data;

    // Заполнение структуры показаний датчика:
    result = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
    if(result == BME280_OK)
    {
        data->temperature = comp_data.temperature;
        data->pressure = comp_data.pressure;
        data->humidity = comp_data.humidity;
    }
    else
    {
        Error_Handler();
    }
}
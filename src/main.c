#include "stm32f4xx_hal.h"
#include "board.h"

int main(void)
{
    // Инициализация HAL:
    HAL_Init();

    // Настройка тактирования:
    SystemClock_Config();

    // Инициализация периферии:
    MX_GPIO_Init();
    MX_USART3_UART_Init();
    MX_I2C1_Init();

    while (1)
    {
        // Главный цикл.
    }
}
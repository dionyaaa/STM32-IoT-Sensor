#include "stm32f4xx_hal.h"

#include <stdint.h> // Для типа uint32_t.

#include "config.h"
#include "board.h"

// Инициализация глобальных структур:
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
I2C_HandleTypeDef hi2c1;

// Инициализация статических переменных:
static uint32_t ticks_per_us;

// Функция выхода в ошибку:
void Error_Handler(void)
{
    __disable_irq();
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_PIN, GPIO_PIN_SET); // Горит синий светодиод - возникла ошибка.
    while (1)
    {
        // Зависание в цикле.
    }
}

// Функция обработчика системного таймера:
void SysTick_Handler(void)
{
    HAL_IncTick();
}

// Функция счётчика микросекундных пауз:
void Delay_us(uint32_t us)
{
    const uint32_t target_ticks = us * ticks_per_us;
    const uint32_t start_ticks = DWT->CYCCNT;

    while((DWT->CYCCNT - start_ticks) < target_ticks)
    {
        __NOP();
    }
}

// Функция настройки тактирования:
void SystemClock_Config(void)
{
    // Инициализация структур:
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Настройка осциллятора:
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    // Настройки PLL:
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 80;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    // Настройка шин:
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // 80 МГц
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // 80 МГц
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // 40 МГц
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; // 80 МГц

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

// Функция инициализации счётчика DWT:
void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    ticks_per_us = SystemCoreClock / 1000000U;
}

// Функция инициализации GPIO:
void MX_GPIO_Init(void)
{
    // Инициализация структур:
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Включение тактирования портов:
    RCC_GPIO_PORT_CLK_ENABLE(LED_GPIO_PORT);
    RCC_GPIO_PORT_CLK_ENABLE(BUTTON_GPIO_PORT);
    RCC_GPIO_PORT_CLK_ENABLE(DISPLAY_GPIO_PORT);

    // Настройка пинов светодиодов:
    GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN | LED3_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

    // Выключение пинов светодиодов:
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN | LED2_PIN | LED3_PIN, GPIO_PIN_RESET);

    // Настройка пина кнопки:
    GPIO_InitStruct.Pin = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = BUTTON_PULL;
    HAL_GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStruct);

    // Настройка пинов дисплея:
    GPIO_InitStruct.Pin = DISPLAY_RS_PIN | DISPLAY_E_PIN | DISPLAY_DB4_PIN |
                        DISPLAY_DB5_PIN | DISPLAY_DB6_PIN | DISPLAY_DB7_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DISPLAY_GPIO_PORT, &GPIO_InitStruct);

    // Выключение пинов дисплея:
    HAL_GPIO_WritePin(DISPLAY_GPIO_PORT, DISPLAY_RS_PIN | DISPLAY_E_PIN | DISPLAY_DB4_PIN |
                    DISPLAY_DB5_PIN | DISPLAY_DB6_PIN | DISPLAY_DB7_PIN, GPIO_PIN_RESET);
}

// Функции инициализации USART2 и USART3:
void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = UART_NETWORK_BAUDRATE;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

void MX_USART3_UART_Init(void)
{
    huart3.Instance = USART3;
    huart3.Init.BaudRate = UART_DEBUG_BAUDRATE;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart3) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(huart->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_ENABLE();
        RCC_GPIO_PORT_CLK_ENABLE(UART_NETWORK_GPIO_PORT);

        GPIO_InitStruct.Pin = UART_NETWORK_TX_PIN | UART_NETWORK_RX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = UART_NETWORK_AF;
        HAL_GPIO_Init(UART_NETWORK_GPIO_PORT, &GPIO_InitStruct);
    }
    else if(huart->Instance == USART3)
    {
        __HAL_RCC_USART3_CLK_ENABLE();
        RCC_GPIO_PORT_CLK_ENABLE(UART_DEBUG_GPIO_PORT);

        GPIO_InitStruct.Pin = UART_DEBUG_TX_PIN | UART_DEBUG_RX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = UART_DEBUG_AF;
        HAL_GPIO_Init(UART_DEBUG_GPIO_PORT, &GPIO_InitStruct);
    }
}

// Функции инициализации I2C1:
void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = I2C1_CLOCK_SPEED;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(hi2c->Instance == I2C1)
    {
        __HAL_RCC_I2C1_CLK_ENABLE();
        RCC_GPIO_PORT_CLK_ENABLE(I2C1_GPIO_PORT);

        GPIO_InitStruct.Pin = I2C1_SCL_PIN | I2C1_SDA_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = I2C1_AF;
        HAL_GPIO_Init(I2C1_GPIO_PORT, &GPIO_InitStruct);
    }
}
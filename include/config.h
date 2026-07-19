#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f4xx_hal.h"

/*                 LED CONFIGURATION:                 */
#define LED_GPIO_PORT               GPIOB
#define LED1_PIN                    GPIO_PIN_0
#define LED2_PIN                    GPIO_PIN_7
#define LED3_PIN                    GPIO_PIN_14

/*                BUTTON CONFIGURATION:               */
#define BUTTON_GPIO_PORT            GPIOC
#define BUTTON_PIN                  GPIO_PIN_13
#define BUTTON_PULL                 GPIO_PULLDOWN

/*             USART3/DEBUG CONFIGURATION:            */
#define UART_DEBUG_GPIO_PORT        GPIOD
#define UART_DEBUG_TX_PIN           GPIO_PIN_8
#define UART_DEBUG_RX_PIN           GPIO_PIN_9
#define UART_DEBUG_AF               GPIO_AF7_USART3
#define UART_DEBUG_BAUDRATE         115200

/*            USART2/NETWORK CONFIGURATION:           */
#define UART_NETWORK_GPIO_PORT      GPIOD
#define UART_NETWORK_TX_PIN         GPIO_PIN_5
#define UART_NETWORK_RX_PIN         GPIO_PIN_6
#define UART_NETWORK_AF             GPIO_AF7_USART2
#define UART_NETWORK_BAUDRATE       115200

/*                I2C1 CONFIGURATION:                 */
#define I2C1_GPIO_PORT              GPIOB
#define I2C1_SCL_PIN                GPIO_PIN_8
#define I2C1_SDA_PIN                GPIO_PIN_9
#define I2C1_AF                     GPIO_AF4_I2C1
#define I2C1_CLOCK_SPEED            100000

/*               DISPLAY CONFIGURATION:               */
#define DISPLAY_GPIO_PORT           GPIOE
#define DISPLAY_RS_PIN              GPIO_PIN_8
#define DISPLAY_E_PIN               GPIO_PIN_7
#define DISPLAY_DB4_PIN             GPIO_PIN_10
#define DISPLAY_DB5_PIN             GPIO_PIN_12
#define DISPLAY_DB6_PIN             GPIO_PIN_14
#define DISPLAY_DB7_PIN             GPIO_PIN_15

/*               NETWORK CONFIGURATION:               */
#if __has_include("config_secret.h")
    #include "config_secret.h"
#else
    #define WIFI_SSID               "YOUR_SSID"
    #define WIFI_PASS               "YOUR_PASSWORD"

    #define HOST                    "127.0.0.1"
    #define PORT                    443
    #define PATH                    "/receive"

    #define SENSOR_ID               "SENSOR_XXXXXXXXXXXXXXXX"
#endif

/*      Enable clock for the specified GPIO port:      */
#define RCC_GPIO_PORT_CLK_ENABLE(port) \
    do { \
        if((port) == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE(); \
        else if((port) == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE(); \
        else if((port) == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE(); \
        else if((port) == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE(); \
        else if((port) == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE(); \
        else if((port) == GPIOF) __HAL_RCC_GPIOF_CLK_ENABLE(); \
        else if((port) == GPIOG) __HAL_RCC_GPIOG_CLK_ENABLE(); \
        else if((port) == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE(); \
        else{} \
    } while (0)

#endif
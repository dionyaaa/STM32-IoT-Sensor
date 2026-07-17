#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f4xx_hal.h"

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

#endif
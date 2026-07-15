#ifndef CONFIG_H
#define CONFIG_H

#if __has_include("config_secret.h")
    #include "config_secret.h"
#else
    #define WIFI_SSID "YOUR_SSID"
    #define WIFI_PASS "YOUR_PASSWORD"

    #define HOST "127.0.0.1"
    #define PORT 5000
    #define PATH "/receive"

    #define SENSOR_ID "SENSOR_XXXXXXXXXXXXXXXX"
#endif

#endif
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern float glob_temperature;
extern float glob_humidity;

extern String WIFI_SSID;
extern String WIFI_PASS;
extern String CORE_IOT_TOKEN;
extern String CORE_IOT_SERVER;
extern String CORE_IOT_PORT;

extern String ssid ;
extern String password ;
extern String wifi_ssid ;
extern String wifi_password ;

extern boolean isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet;

typedef struct {
    float temperature;
    float humidity;
} SensorData;

extern QueueHandle_t qSensorData;
extern SemaphoreHandle_t semSensorData;
#endif
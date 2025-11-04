#ifndef __TASK_CORE_IOT_H__
#define __TASK_CORE_IOT_H__

#include <WiFi.h>
#include <ThingsBoard.h>
#include <Arduino_MQTT_Client.h>
#include <HTTPClient.h>
#include "task_check_info.h"

void CORE_IOT_sendata(String mode, String feed, String data);
void CORE_IOT_reconnect();

#endif
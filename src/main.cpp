#include "global.h"

#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi_monitor.h"
#include "mainserver.h"
#include "tinyml.h"
#include "coreiot.h"

// include task
// #include "task_check_info.h"
// #include "task_core_iot.h"
#include "task_toogle_boot.h"
// #include "task_webserver.h"
// #include "task_wifi.h"

#include "dht20_sensor.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("Setup started");
    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, LOW);

    ToggleData data;
    data.toggleStateLed1 = true;
    data.toggleStateLed2 = true;
    xQueueOverwrite(qToggleState, &data);

    xTaskCreate(led_blinky, "Task LED Blink", 2048, NULL, 2, NULL);
    xTaskCreate(neo_blinky, "Task NEO Blink", 2048, NULL, 2, NULL);
    xTaskCreate(dht20_sensor_task, "Task DHT20 Sensor", 4096, NULL, 2, NULL);
    xTaskCreate(temp_humi_monitor, "Task TEMP HUMI Monitor", 2048, NULL, 2, NULL);
    xTaskCreate(main_server_task, "Task Main Server", 8192, NULL, 2, NULL);
    xTaskCreate(tiny_ml_task, "Tiny ML Task", 2048, NULL, 2, NULL);
    xTaskCreate(coreiot_task, "CoreIOT Task", 4096, NULL, 2, NULL);
    xTaskCreate(Task_Toogle_BOOT, "Task_Toogle_BOOT", 4096, NULL, 2, NULL);
}

void loop()
{
    // if (check_info_File(1))
    // {
    //     if (!Wifi_reconnect())
    //     {
    //         Webserver_stop();
    //     }
    //     else
    //     {
    //         CORE_IOT_reconnect();
    //     }
    // }
    // Webserver_reconnect();
}
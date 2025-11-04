#include "led_blinky.h"

void led_blinky(void *pvParameters){
  SensorData recv;
    pinMode(LED_GPIO, OUTPUT);

    while(1){
        if(xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE){
            if(xQueueReceive(qSensorData, &recv, 0) == pdTRUE){
                
                if(recv.humidity >= 60 && recv.humidity <= 70) digitalWrite(LED_GPIO, LOW);
                else if(recv.humidity >= 70) digitalWrite(LED_GPIO, HIGH), vTaskDelay(500);
                else digitalWrite(LED_GPIO, HIGH), vTaskDelay(100);
            }
        }
    }
}
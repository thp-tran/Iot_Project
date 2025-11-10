#include "led_blinky.h"

void led_blinky(void *pvParameters){
  SensorData recv0;

    while(1){
        if(xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE){
            if(xQueuePeek(qSensorData, &recv0, 0) == pdTRUE){
                
                if(recv0.humidity >= 60 && recv0.humidity <= 70) digitalWrite(LED_GPIO, LOW);
                else if(recv0.humidity >= 70) digitalWrite(LED_GPIO, HIGH), vTaskDelay(500);
                else digitalWrite(LED_GPIO, HIGH), vTaskDelay(100);
            }
        }
    }
}
#include "led_blinky.h"

void led_blinky(void *pvParameters){
    pinMode(LED_GPIO, OUTPUT);
  
  while(1) {           
    if (xSemaphoreTake(tempSemaphore, portMAX_DELAY) == pdTRUE) {
        int blinkDelay;

        if (glob_temperature < 25) {
            blinkDelay = 1000; // chớp chậm
            Serial.println("LED: Cool (Slow blink)");
        } else if (glob_temperature < 29) {
            blinkDelay = 500;  // chớp vừa
            Serial.println("LED: Warm (Medium blink)");
        } else {
            blinkDelay = 200;  // chớp nhanh
            Serial.println("LED: Hot (Fast blink)");
        }

        // Nhấp nháy vài lần mỗi lần có dữ liệu mới
        for (int i = 0; i < 3; i++) {
            digitalWrite(LED_GPIO, HIGH);
            vTaskDelay(blinkDelay / portTICK_PERIOD_MS);
            digitalWrite(LED_GPIO, LOW);
            vTaskDelay(blinkDelay / portTICK_PERIOD_MS);
        }
    }
  }
}
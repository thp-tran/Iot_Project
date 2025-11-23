#include "led_blinky.h"

void led_blinky(void *pvParameters)
{

    SensorData recv0;

    while (1)
    {

        // Nếu nhận được tín hiệu tắt LED
        uint32_t cmd = 0;
        if (xTaskNotifyWait(0, 0, &cmd, 300) == pdPASS)
        {
            if (cmd == 1)
            {
                digitalWrite(LED_GPIO, LOW);
                continue; // bỏ phần blink phía dưới
            }
        }

        // Blink theo độ ẩm
        if (xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE)
        {
            xSemaphoreGive(semSensorData);
            if (xQueuePeek(qSensorData, &recv0, 0) == pdTRUE)
            {

                // Humidity 60 - 70 → LED OFF
                if (recv0.humidity >= 60 && recv0.humidity <= 70)
                {
                    digitalWrite(LED_GPIO, HIGH);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    digitalWrite(LED_GPIO, LOW);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }

                // Humidity >= 70 → Blink chậm
                else if (recv0.humidity >= 70)
                {
                    digitalWrite(LED_GPIO, HIGH);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    digitalWrite(LED_GPIO, LOW);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }

                // Humidity < 60 → Blink nhanh
                else
                {
                    digitalWrite(LED_GPIO, HIGH);
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                    digitalWrite(LED_GPIO, LOW);
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                }
            }
        }
    }
}

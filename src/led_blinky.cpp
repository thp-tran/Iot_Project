#include "led_blinky.h"

void led_blinky(void *pvParameters)
{

    SensorData recv0;
    ToggleData toggleData;

    while (1)
    {
        if (xQueuePeek(qToggleState, &toggleData, 0) == pdTRUE)
        {
            if (!toggleData.toggleStateLed1)
            {
                digitalWrite(LED_GPIO, LOW);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue; // Skip blinking if both LEDs are off
            }
        }

        // Blink theo độ ẩm
        if (xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE)
        {
            if (xQueuePeek(qSensorData, &recv0, 0) == pdTRUE)
            {
                xSemaphoreGive(semSensorData);
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

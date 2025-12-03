#include "led_blinky.h"

void led_blinky(void *pvParameters)
{

    SensorData recv0;
    ToggleData toggleData;

    while (1)
    {
        // Kiểm tra trạng thái LED từ queue
        if (xQueuePeek(qToggleState, &toggleData, 0) == pdTRUE)
        {
            if (!toggleData.toggleStateLed1)
            {
                digitalWrite(LED_GPIO, LOW);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue; // Skip blinking if both LEDs are off
            }
        }

        // Blink theo nhiệt độ
        if (xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE)
        {
            if (xQueuePeek(qSensorData, &recv0, 0) == pdTRUE)
            {
                xSemaphoreGive(semSensorData);
                // Temperature < 20 → Blink rất chậm
                if (recv0.temperature < 20)
                {
                    digitalWrite(LED_GPIO, HIGH);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    digitalWrite(LED_GPIO, LOW);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }

                // Temperature 20-30 → Blink chậm
                else if (recv0.temperature > 20 && recv0.temperature <= 30)
                {
                    digitalWrite(LED_GPIO, HIGH);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    digitalWrite(LED_GPIO, LOW);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }

                // Temperature 30-40 → Blink nhanh
                else if (recv0.temperature > 30 && recv0.temperature <= 40)
                {
                    digitalWrite(LED_GPIO, HIGH);
                    vTaskDelay(250 / portTICK_PERIOD_MS);
                    digitalWrite(LED_GPIO, LOW);
                    vTaskDelay(250 / portTICK_PERIOD_MS);
                }
            }
        }
    }
}

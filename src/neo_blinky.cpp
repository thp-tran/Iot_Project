#include "neo_blinky.h"
#include "global.h"

void neo_blinky(void *pvParameters)
{
    SensorData recv;
    ToggleData toggleData;

    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.clear();
    strip.show();

    while (1)
    {
        if (xQueuePeek(qToggleState, &toggleData, 0) == pdTRUE)
        {
            if (!toggleData.toggleStateLed2)
            {
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue; // Skip blinking if both LEDs are off
            }
        }

        if (xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE)
        {
            if (xQueuePeek(qSensorData, &recv, portMAX_DELAY) == pdTRUE)
            {
                xSemaphoreGive(semSensorData);
                if (recv.humidity < 50)
                    strip.setPixelColor(0, strip.Color(0, 0, 255));
                else if (recv.humidity <= 75)
                    strip.setPixelColor(0, strip.Color(0, 255, 0));
                else
                    strip.setPixelColor(0, strip.Color(255, 0, 0));
                strip.show();
            }
        }
        vTaskDelay(1000);
    }
}

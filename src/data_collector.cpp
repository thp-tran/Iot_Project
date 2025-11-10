#include "global.h"
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

void data_collector(void *pvParameters)
{
    SensorData recv0;

    // --- Mount hệ thống file ---
    if (!LittleFS.begin(true)) {
        Serial.println("[ERROR] Failed to mount LittleFS!");
        vTaskDelete(NULL);
    }

    // --- Mở file CSV để ghi ---
    File file = LittleFS.open("/dataset.csv", FILE_WRITE);
    if (!file) {
        Serial.println("[ERROR] Cannot open dataset.csv");
        vTaskDelete(NULL);
    }

    Serial.println("=== Data Collector Task Started ===");
    Serial.println("temperature,humidity,label");

    while (1)
    {
        if (xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE)
        {
            if (xQueuePeek(qSensorData, &recv0, 0) != pdTRUE)
            {
                int temperature = (int)(recv0.temperature);
                int humidity = (int)(recv0.humidity);
                
                int label = (temperature > 35 || temperature < 25 || humidity > 80 || humidity < 40) ? 1 : 0;
        
                // Ghi vào file CSV
                file.printf("%d,%d,%d\n", temperature, humidity, label);
                file.flush();
        
                // In ra Serial Monitor
                Serial.printf("%d,%d,%d\n", temperature, humidity, label);

                vTaskDelay(pdMS_TO_TICKS(2000));
            }
        }
    }

    file.close();
}

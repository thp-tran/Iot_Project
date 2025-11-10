#include "dht20_sensor.h"

DHT20 dht20;
void dht20_sensor_task(void *pvParameters) {
    SensorData data;
    // Serial.begin(115200);
    Wire.begin(11, 12);
    dht20.begin();
    Serial.println("DHT20 Sensor Task Started");

    while (1) {
        dht20.read();
        data.temperature = dht20.getTemperature();
        data.humidity = dht20.getHumidity();

        if (isnan(data.temperature) || isnan(data.humidity)) {
            // Handle read failure
            data.temperature = -1;
            data.humidity = -1;
        }

        Serial.printf("Temperature: %.2f °C, Humidity: %.2f %%\n", data.temperature, data.humidity);

        xQueueOverwrite(qSensorData, &data);
        xSemaphoreGive(semSensorData);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
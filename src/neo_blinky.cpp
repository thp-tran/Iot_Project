#include "neo_blinky.h"
#include "global.h"

void neo_blinky(void *pvParameters){

    SensorData recv;
    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.clear();
    strip.show();

    while(1){
        // chờ có sensor update
        if(xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE){
            if(xQueuePeek(qSensorData, &recv, 0) == pdTRUE){

                if(recv.humidity < 50){
                    strip.setPixelColor(0, strip.Color(0, 0, 255)); // Blue = Dry
                }
                else if(recv.humidity <= 75){
                    strip.setPixelColor(0, strip.Color(0, 255, 0)); // Green = OK
                }
                else{
                    strip.setPixelColor(0, strip.Color(255, 0, 0)); // Red = Wet
                }

                strip.show();
            }
        }
    }
}
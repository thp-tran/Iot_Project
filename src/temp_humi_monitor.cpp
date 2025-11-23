#include "temp_humi_monitor.h"
LiquidCrystal_I2C lcd(33,16,2);

void temp_humi_monitor(void *pvParameters){
    SensorData recv0;
    lcd.begin();
    lcd.backlight();

    while(1){
        if(xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE){
            if(xQueuePeek(qSensorData, &recv0, 0) == pdTRUE){
                xSemaphoreGive(semSensorData);
                // LCD display with 3 states
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("T: ");
                lcd.print(recv0.temperature);
                lcd.print(" C ");
        
                lcd.setCursor(0,1);
                lcd.print("H: ");
                lcd.print(recv0.humidity);
                lcd.print(" %");
        
                // Condition states on LCD
                if(recv0.temperature < 30){
                    lcd.setCursor(10,0);
                    lcd.print(" OK");
                }
                else if(recv0.temperature < 40){
                    lcd.setCursor(10,0);
                    lcd.print(" WARN");
                }
                else{
                    lcd.setCursor(10,0);
                    lcd.print(" HOT!");
                }
        
                if(recv0.humidity < 50){
                    lcd.setCursor(10,1);
                    lcd.print(" DRY");
                }
                else if(recv0.humidity >= 50 && recv0.humidity <= 75){
                    lcd.setCursor(10,1);
                    lcd.print(" OK");
                }
                else{
                    lcd.setCursor(10,1);
                    lcd.print(" WET!");
                }
            }
        }
        
        vTaskDelay(1000);
    }
}

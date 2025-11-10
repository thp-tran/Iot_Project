#include "temp_humi_monitor.h"
LiquidCrystal_I2C lcd(33,16,2);


// void temp_humi_monitor(void *pvParameters){

//     Wire.begin(11, 12);
//     Serial.begin(115200);
//     dht20.begin();

//     while (1){
//         /* code */
        
//         dht20.read();
//         // Reading temperature in Celsius
//         float temperature = dht20.getTemperature();
//         // Reading humidity
//         float humidity = dht20.getHumidity();

        

//         // Check if any reads failed and exit early
//         if (isnan(temperature) || isnan(humidity)) {
//             Serial.println("Failed to read from DHT sensor!");
//             temperature = humidity =  -1;
//             //return;
//         }

//         //Update global variables for temperature and humidity
//         glob_temperature = temperature;
//         glob_humidity = humidity;

//         // Print the results
        
//         Serial.print("Humidity: ");
//         Serial.print(humidity);
//         Serial.print("%  Temperature: ");
//         Serial.print(temperature);
//         Serial.println("°C");
        
//         vTaskDelay(5000);
//     }
    
// }

void temp_humi_monitor(void *pvParameters){
    SensorData recv0;
    lcd.begin();
    lcd.backlight();

    while(1){
        if(xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE){
            if(xQueuePeek(qSensorData, &recv0, 0) == pdTRUE){

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

    }
}

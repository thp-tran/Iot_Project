#include "temp_humi_monitor.h"
DHT20 dht20;
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
    SensorData data;
    Wire.begin(11, 12);
    Serial.begin(115200);
    dht20.begin();
    lcd.begin();
    lcd.backlight();

    while(1){
        dht20.read();
        data.temperature = dht20.getTemperature();
        data.humidity = dht20.getHumidity();

        if (isnan(data.temperature) || isnan(data.humidity)) {
            data.temperature = data.humidity = -1;
        }

        // Send sensor data to queue
        xQueueOverwrite(qSensorData, &data);

        // LCD display with 3 states
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("T: ");
        lcd.print(data.temperature);
        lcd.print(" C ");

        lcd.setCursor(0,1);
        lcd.print("H: ");
        lcd.print(data.humidity);
        lcd.print(" %");

        // Condition states on LCD
        if(data.temperature < 30){
            lcd.setCursor(10,0);
            lcd.print(" OK");
        }
        else if(data.temperature < 40){
            lcd.setCursor(10,0);
            lcd.print(" WARN");
        }
        else{
            lcd.setCursor(10,0);
            lcd.print(" HOT!");
        }

        if(data.humidity < 50){
            lcd.setCursor(10,1);
            lcd.print(" DRY");
        }
        else if(data.humidity >= 50 && data.humidity <= 75){
            lcd.setCursor(10,1);
            lcd.print(" OK");
        }
        else{
            lcd.setCursor(10,1);
            lcd.print(" WET!");
        }

        // Notify other tasks that new data exists

        Serial.print("Temp: "); Serial.print(data.temperature);
        Serial.print("C  Humi: "); Serial.println(data.humidity);

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

#include "coreiot.h"

// ----------- CONFIGURE THESE! -----------
const char* coreIOT_Server = "app.coreiot.io";  
const char* coreIOT_Token = "s2Mj1H6hB2NsfksxLppO";   // Device Access Token
const int   mqttPort = 1883;
// ----------------------------------------

WiFiClient espClient;
PubSubClient client(espClient);


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect (username=token, password=empty)
    if (client.connect("ESP32Client", coreIOT_Token, NULL)) {
      Serial.println("connected to CoreIOT Server!");
      client.subscribe("v1/devices/me/rpc/request/+");
      Serial.println("Subscribed to v1/devices/me/rpc/request/+");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


// void callback(char* topic, byte* payload, unsigned int length) {
//   Serial.print("Message arrived [");
//   Serial.print(topic);
//   Serial.println("] ");

//   // Allocate a temporary buffer for the message
//   char message[length + 1];
//   memcpy(message, payload, length);
//   message[length] = '\0';
//   Serial.print("Payload: ");
//   Serial.println(message);

//   // Parse JSON
//   StaticJsonDocument<256> doc;
//   DeserializationError error = deserializeJson(doc, message);

//   if (error) {
//     Serial.print("deserializeJson() failed: ");
//     Serial.println(error.c_str());
//     return;
//   }

//   const char* method = doc["method"];
//   if (strcmp(method, "setStateLED") == 0) {
//     // Check params type (could be boolean, int, or string according to your RPC)
//     // Example: {"method": "setValueLED", "params": "ON"}
//     const char* params = doc["params"];

//     if (strcmp(params, "ON") == 0) {
//       Serial.println("Device turned ON.");
//       digitalWrite(LED_GPIO, HIGH);
//       //TODO
//     } else {   
//       Serial.println("Device turned OFF.");
//       digitalWrite(LED_GPIO, LOW);
//       //TODO

//     }
//   } else {
//     Serial.print("Unknown method: ");
//     Serial.println(method);
//   }
// }


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("]");

  // 🔒 Kiểm tra dữ liệu rỗng
  if (length == 0) {
    Serial.println("⚠️ Empty payload received!");
    return;
  }

  // 🔒 Copy payload an toàn sang chuỗi
  static char message[512];  // bộ đệm đủ lớn, tránh cấp phát stack
  if (length >= sizeof(message)) length = sizeof(message) - 1;
  memcpy(message, payload, length);
  message[length] = '\0';

  Serial.print("Payload: ");
  Serial.println(message);

  // 🔒 Parse JSON an toàn
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("❌ JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }

  // 🔒 Lấy "method" và kiểm tra null
  const char* method = doc["method"];
  if (!method) {
    Serial.println("⚠️ Missing 'method' in JSON!");
    return;
  }

  // Xử lý RPC "setStateLED"
  if (strcmp(method, "setStateLED") == 0) {
    Serial.println("✅ RPC: setStateLED");

    if (doc["params"].is<bool>()) {
      bool state = doc["params"];
      digitalWrite(LED_GPIO, state ? HIGH : LOW);
      Serial.println(state ? "LED ON (bool)" : "LED OFF (bool)");
      String feedback = "{\"ledState\":";
      feedback += (state ? "true" : "false");
      feedback += "}";
      client.publish("v1/devices/me/attributes", feedback.c_str());
      Serial.println("Published LED state to server: " + feedback);

    } else if (doc["params"].is<const char*>()) {
      const char* param = doc["params"];
      if (param && strcmp(param, "ON") == 0) {
        digitalWrite(LED_GPIO, HIGH);
        Serial.println("LED ON (string)");
      } else {
        digitalWrite(LED_GPIO, LOW);
        Serial.println("LED OFF (string)");
      }

    } else {
      Serial.println("⚠️ Unknown param type!");
    }

  } else if(strcmp(method, "getStateLED") == 0){
    
  }
  else {
    Serial.print("⚠️ Unknown method: ");
    Serial.println(method);
  }
}


void setup_coreiot(){

  //Serial.print("Connecting to WiFi...");
  //WiFi.begin(wifi_ssid, wifi_password);
  //while (WiFi.status() != WL_CONNECTED) {
  
  // while (isWifiConnected == false) {
  //   delay(500);
  //   Serial.print(".");
  // }

  while(1){
    if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY)) {
      break;
    }
    delay(500);
    Serial.print(".");
  }


  Serial.println(" Connected!");

  client.setServer(coreIOT_Server, mqttPort);
  client.setCallback(callback);

}

void coreiot_task(void *pvParameters){

    setup_coreiot();

    while(1){

        if (!client.connected()) {
            reconnect();
        }
        client.loop();
        float hum;
        float temp;
        SensorData recv2;
        if(xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE){
            if(xQueuePeek(qSensorData, &recv2, 0) == pdTRUE){
              hum = recv2.humidity;
              temp = recv2.temperature;
              String payload = "{\"temperature\":" + String(temp) +  ",\"humidity\":" + String(hum) + "}";
        
              client.publish("v1/devices/me/telemetry", payload.c_str());

              Serial.println("Published payload: " + payload);
            }
        }
        // Sample payload, publish to 'v1/devices/me/telemetry'
        // String payload = "{\"temperature\":" + String(glob_temperature) +  ",\"humidity\":" + String(glob_humidity) + "}";
        
        // client.publish("v1/devices/me/telemetry", payload.c_str());

        // Serial.println("Published payload: " + payload);
        vTaskDelay(10000);  // Publish every 10 seconds
    }
}
#include "coreiot.h"

// ===============================
// CONFIG LOCAL + COREIOT
// ===============================
const char* LOCAL_MQTT_IP = "127.0.0.1";     // Local Mosquitto
const int   LOCAL_MQTT_PORT = 1883;

const char* COREIOT_SERVER = "app.coreiot.io";  
const char* COREIOT_TOKEN  = "s2Mj1H6hB2NsfksxLppO";
const int   COREIOT_PORT   = 1883;

bool useLocalMQTT = false;

WiFiClient espClient;
PubSubClient client(espClient);

// ===============================
// RECONNECT cho Local và CoreIoT
// ===============================
bool try_connect_local() {
    client.setServer(LOCAL_MQTT_IP, LOCAL_MQTT_PORT);

    Serial.println("[MQTT] Trying LOCAL broker...");
    if (client.connect("ESP32_Local")) {
        Serial.println("[MQTT] Connected to LOCAL broker!");
        return true;
    }
    Serial.println("[MQTT] Local broker unavailable.");
    return false;
}

bool try_connect_coreiot() {
    client.setServer(COREIOT_SERVER, COREIOT_PORT);

    Serial.println("[MQTT] Trying CoreIoT broker...");
    if (client.connect("ESP32_CoreIoT", COREIOT_TOKEN, NULL)) {
        Serial.println("[MQTT] Connected to CoreIoT!");
        client.subscribe("v1/devices/me/rpc/request/+");
        return true;
    }
    Serial.println("[MQTT] CoreIoT connect failed.");
    return false;
}

void smart_reconnect() {
    while (!client.connected()) {
        Serial.println("[MQTT] Reconnecting...");
        
        // 1) thử local trước
        if (try_connect_local()) {
            useLocalMQTT = true;
            return;
        }

        // 2) nếu local fail → thử CoreIoT
        if (try_connect_coreiot()) {
            useLocalMQTT = false;
            return;
        }

        Serial.println("[MQTT] Retry in 5s...");
        delay(5000);
    }
}

// ===============================
// CALLBACK (giữ NGUYÊN phần RPC)
// ===============================
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.println("]");

    if (length == 0) return;

    static char message[512];
    if (length >= sizeof(message)) length = sizeof(message) - 1;
    memcpy(message, payload, length);
    message[length] = '\0';

    Serial.print("Payload: ");
    Serial.println(message);

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) return;

    const char* method = doc["method"];
    if (!method) return;

    // ======= RPC giữ nguyên như gốc =======
    if (strcmp(method, "setStateLED") == 0) {
        Serial.println("RPC: setStateLED");

        if (doc["params"].is<bool>()) {
            bool state = doc["params"];
            digitalWrite(LED_GPIO, state ? HIGH : LOW);

            String feedback = "{\"ledState\":";
            feedback += (state ? "true" : "false");
            feedback += "}";
            client.publish("v1/devices/me/attributes", feedback.c_str());
        }
    }
}

// ===============================
// SETUP
// ===============================
void setup_coreiot() {

    // đợi có internet
    while (1) {
        if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY))
            break;
        delay(500);
    }

    Serial.println("Connected WiFi");

    client.setCallback(callback);

    // auto connect local → cloud
    smart_reconnect();
}

// ===============================
// TASK LOOP
// ===============================
void coreiot_task(void *pvParameters) {

    setup_coreiot();

    while (1) {

        if (!client.connected()) {
            smart_reconnect();
        }

        client.loop();

        float hum, temp;
        SensorData recv2;

        if (xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE) {
            if (xQueuePeek(qSensorData, &recv2, 0) == pdTRUE) {

                hum = recv2.humidity;
                temp = recv2.temperature;

                String payload = "{\"temperature\":" + String(temp) +
                                 ",\"humidity\":" + String(hum) + "}";

                if (useLocalMQTT) {
                    client.publish("esp32/telemetry", payload.c_str());
                    Serial.println("LOCAL MQTT → " + payload);

                } else {
                    client.publish("v1/devices/me/telemetry", payload.c_str());
                    Serial.println("CoreIoT → " + payload);
                }
            }
        }

        vTaskDelay(3000);
    }
}

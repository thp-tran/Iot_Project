#include "coreiot.h"

// ----------- CONFIGURE THESE! -----------
const char *coreIOT_Server = "app.coreiot.io";      // CoreIOT Server
const char *coreIOT_Token = "8mkMy9rgY6x6lq6ih0tZ"; // Device Access Token
IPAddress ip(192, 168, 102, 195);
const int mqttPort = 1883;
// ----------------------------------------
bool isLocalServer = false; // Set to true if using a local CoreIOT server
WiFiClient espClient;
PubSubClient client(espClient);

void sendLedState()
{
    ToggleData d;
    xQueuePeek(qToggleState, &d, 0);
    StaticJsonDocument<128> doc;
    doc["led1"] = d.toggleStateLed1;
    doc["led2"] = d.toggleStateLed2;
    String attr;
    serializeJson(doc, attr);
    if (!isLocalServer)
    {
        client.publish("v1/devices/me/attributes", attr.c_str());
        Serial.println("Sent LED attributes on connect: " + attr);
    }
    else
    {
        client.publish("esp32/attributes", attr.c_str());
        Serial.println("Sent LED attributes on connect to Local CoreIOT: " + attr);
    }
}

void reconnect()
{
    float crrtime = millis();
    // Loop until we're reconnected
    while (!client.connected())
    {
        if (isLocalServer && (millis() - crrtime > 10000))
        {
            // After 60s of trying to connect to local server, switch to cloud
            Serial.println("Switching to CoreIOT Cloud server...");
            client.setServer(coreIOT_Server, mqttPort);
            isLocalServer = false;
        }
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect (username=token, password=empty)
        if (client.connect("ESP32Client", coreIOT_Token, NULL))
        {
            Serial.println("connected to CoreIOT Server!");
            if (isLocalServer)
            {
                client.subscribe("esp32/devices/me/rpc/request/");
                Serial.println("Subscribed to esp32/devices/me/rpc/request/+ on Local CoreIOT");
            }
            else
            {
                client.subscribe("v1/devices/me/rpc/request/+");
                Serial.println("Subscribed to v1/devices/me/rpc/request/+");
            }
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
    sendLedState();
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.println("]");

    if (length == 0)
        return;

    static char message[512];
    if (length >= sizeof(message))
        length = sizeof(message) - 1;
    memcpy(message, payload, length);
    message[length] = '\0';

    Serial.print("Payload: ");
    Serial.println(message);

    StaticJsonDocument<512> doc;
    if (deserializeJson(doc, message))
    {
        Serial.println("JSON parse error");
        return;
    }

    const char *method = doc["method"];
    bool param = false;
    if (!method)
    {
        JsonObject data = doc["data"];
        if (!data.isNull())
        {
            method = data["method"];
            param = data["params"];
        }
    }
    else
    {
        param = doc["params"];
    }

    // Không có method → bỏ qua
    if (!method)
    {
        Serial.println("No method field!");
        return;
    }

    Serial.print("RPC Method: ");
    Serial.println(method);

    // Đọc trạng thái LED đang lưu trong queue
    ToggleData state;
    xQueuePeek(qToggleState, &state, 0);
    // XỬ LÝ LED1
    if (strcmp(method, "setLED1") == 0)
    {
        state.toggleStateLed1 = param;
        Serial.println(param ? "LED1 → ON" : "LED1 → OFF");
    }
    // XỬ LÝ LED2
    else if (strcmp(method, "setLED2") == 0)
    {
        state.toggleStateLed2 = param;
        Serial.println(param ? "LED2 → ON" : "LED2 → OFF");
    }
    // Ghi lại vào queue
    xQueueOverwrite(qToggleState, &state);
    // PUBLISH TRẠNG THÁI 2 LED
    StaticJsonDocument<128> resp;
    resp["led1"] = state.toggleStateLed1;
    resp["led2"] = state.toggleStateLed2;

    String out;
    serializeJson(resp, out);
    client.publish("v1/devices/me/attributes", out.c_str());
    Serial.println("Updated LED attributes: " + out);
}

void setup_coreiot()
{

    // Wait until internet OK
    while (1)
    {
        if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY))
            break;
        Serial.print(".");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    Serial.println(" Internet OK!");

    Serial.println("[MQTT] Trying LOCAL server...");
    client.setServer(ip, mqttPort);

    unsigned long startAttempt = millis();
    isLocalServer = false; // default

    while (millis() - startAttempt < 8000)
    { // Try local within 8s
        if (client.connect("ESP32Client", coreIOT_Token, NULL))
        {
            Serial.println("Connected to LOCAL CoreIOT Server!");
            isLocalServer = true;
            client.subscribe("esp32/telemetry/+");
            client.subscribe("esp32/devices/me/rpc/request/");
            Serial.println("Subscribed to local topics");
            sendLedState();
            break;
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    if (!isLocalServer)
    {
        Serial.println("[MQTT] Local failed → switching to CoreIOT cloud...");
        client.setServer(coreIOT_Server, mqttPort);
    }

    client.setCallback(callback);
}

void coreiot_task(void *pvParameters)
{

    setup_coreiot();

    while (1)
    {
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();
        float hum;
        float temp;
        SensorData recv2;
        if (xSemaphoreTake(semSensorData, portMAX_DELAY) == pdTRUE)
        {
            if (xQueuePeek(qSensorData, &recv2, 0) == pdTRUE)
            {
                hum = recv2.humidity;
                temp = recv2.temperature;
                String payload = "{\"temperature\":" + String(temp) + ",\"humidity\":" + String(hum) + "}";
                if (isLocalServer)
                {

                    client.publish("esp32/telemetry", payload.c_str());

                    Serial.println("Published payload to Local CoreIOT Server: " + payload);
                }
                else
                {
                    client.publish("v1/devices/me/telemetry", payload.c_str());
                }
            }
        }
        vTaskDelay(3000); // Publish every 3 seconds
    }
}
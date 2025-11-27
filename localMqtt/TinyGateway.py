import paho.mqtt.client as mqtt
import json
import time

# ===== LOCAL BROKER =====
LOCAL_HOST = "127.0.0.1"
LOCAL_PORT = 1883

# ===== COREIOT CLOUD =====
COREIOT_HOST = "app.coreiot.io"
COREIOT_PORT = 1883
COREIOT_TOKEN = "HIEV4aTOJ0iVDhLzlYFP"   # token của bạn

# ===== MQTT CLIENTS =====
local_client = mqtt.Client("LocalSub")
cloud_client = mqtt.Client("CoreIOTGateway")
cloud_client.username_pw_set(COREIOT_TOKEN)


# === When receiving message from local broker ===
def on_local_message(client, userdata, msg):
    print("Local →", msg.payload.decode())

    try:
        data = json.loads(msg.payload.decode())
        print(data)

    except:
        print("❌ Invalid JSON")
        return

    gateway_payload = {
        "ESP32_001": [
            {
                "ts": int(time.time() * 1000),
                "values": data
            }
        ]
    }

    cloud_client.publish("v1/gateway/telemetry", json.dumps(gateway_payload))
    print("→ Forwarded to CoreIoT!")


# --- Setup Local Subscriber ---
local_client.on_message = on_local_message
local_client.connect(LOCAL_HOST, LOCAL_PORT)
local_client.subscribe("esp32/telemetry")

# --- Setup Cloud Publisher ---
cloud_client.connect(COREIOT_HOST, COREIOT_PORT)
cloud_client.loop_start()

print("Gateway running...")
local_client.loop_forever()

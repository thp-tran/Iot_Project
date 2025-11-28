import paho.mqtt.client as mqtt
import json
import time

LOCAL_HOST = "127.0.0.1"
LOCAL_PORT = 1883

COREIOT_HOST = "app.coreiot.io"
COREIOT_PORT = 1883
COREIOT_TOKEN = "HIEV4aTOJ0iVDhLzlYFP"

local_client = mqtt.Client("LocalSub")
cloud_client = mqtt.Client("CoreIOTGateway")
cloud_client.username_pw_set(COREIOT_TOKEN)


# ========== TELEMETRY FROM ESP32 → CLOUD ==========
def on_local_telemetry(client, userdata, msg):
    print("Telemetry →", msg.payload.decode())

    try:
        data = json.loads(msg.payload.decode())
    except:
        print("❌ Invalid JSON")
        return

    gateway_payload = {
        "ESP32_001": [{
            "ts": int(time.time() * 1000),
            "values": data
        }]
    }

    cloud_client.publish("v1/gateway/telemetry", json.dumps(gateway_payload))
    print("✔ Forwarded telemetry to cloud")


# ========== ATTRIBUTES FROM ESP32 → CLOUD ==========
def on_local_attributes(client, userdata, msg):
    print("Attributes →", msg.payload.decode())

    try:
        attr = json.loads(msg.payload.decode())
    except:
        print("❌ Invalid attribute JSON")
        return

    cloud_client.publish("v1/devices/me/attributes", json.dumps(attr))
    print("✔ Forwarded attributes to cloud")


# ========== RPC FROM CLOUD → ESP32 ==========
def on_cloud_rpc(client, userdata, msg):
    print("RPC from cloud:", msg.payload.decode())

    try:
        rpc = json.loads(msg.payload.decode())
    except:
        print("❌ Invalid RPC JSON")
        return

    # Forward RPC to local ESP32
    local_client.publish("esp32/rpc", json.dumps(rpc))
    print("✔ RPC forwarded → ESP32")



# ---- SETUP LOCAL SUBSCRIBERS ----
local_client.message_callback_add("esp32/telemetry", on_local_telemetry)
local_client.message_callback_add("esp32/attributes", on_local_attributes)

local_client.connect(LOCAL_HOST, LOCAL_PORT)
local_client.subscribe("esp32/#")       # telemetry + attributes


# ---- SETUP CLOUD SUBSCRIBER (RPC) ----
cloud_client.on_message = on_cloud_rpc
cloud_client.connect(COREIOT_HOST, COREIOT_PORT)
cloud_client.subscribe("v1/devices/me/rpc/request/+")
cloud_client.loop_start()

print("🚀 FULL BI-DIRECTION GATEWAY READY")
local_client.loop_forever()

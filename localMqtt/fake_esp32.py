import paho.mqtt.client as mqtt
import json
import time
import random

BROKER = "127.0.0.1"

client = mqtt.Client("FakeESP32")

led_state = 0   # 0 = OFF, 1 = ON


def on_rpc(client, userdata, msg):
    global led_state
    print("RPC → ESP32:", msg.payload.decode())

    try:
        rpc = json.loads(msg.payload.decode())
    except:
        print("Invalid RPC packet")
        return

    # Extract actual RPC fields:
    method = rpc["data"]["method"]
    params = rpc["data"]["params"]

    # Example: {"method": "setValue", "params": true}
    if method == "setValue":
        led_state = 1 if params else 0
        print("SET LED =", led_state)

        # Report attributes
        attr_payload = {"ledstate": led_state}
        client.publish("esp32/attributes", json.dumps(attr_payload))
        print("↑ Report attributes:", attr_payload)

client.on_message = on_rpc

client.connect(BROKER, 1883)
result, mid = client.subscribe("esp32/rpc")

client.loop_start()

print("Fake ESP32 started...")

try:
    while True:
        temp = round(random.uniform(25, 33), 2)
        hum = round(random.uniform(50, 80), 2)

        payload = {"temperature": temp, "humidity": hum}

        client.publish("esp32/telemetry", json.dumps(payload))
        print("Sent telemetry:", payload)

        time.sleep(2)

except KeyboardInterrupt:
    print("Stopped")

client.loop_stop()
client.disconnect()

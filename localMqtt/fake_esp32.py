import paho.mqtt.client as mqtt
import json
import time
import random

BROKER = "127.0.0.1"
TOPIC = "esp32/telemetry"

client = mqtt.Client("FakeESP32")   # OK nếu dùng paho-mqtt 1.6.1

client.connect(BROKER, 1883)
client.loop_start()

print("Fake ESP32 started, publishing data...")

try:
    while True:
        temp = round(random.uniform(25.0, 33.0), 2)
        hum = round(random.uniform(50.0, 80.0), 2)

        payload = {
            "temperature": temp,
            "humidity": hum
        }

        client.publish(TOPIC, json.dumps(payload))
        print("Sent:", payload)

        time.sleep(2)

except KeyboardInterrupt:
    print("Stopped")

client.loop_stop()
client.disconnect()

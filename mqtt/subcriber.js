const mqtt = require("mqtt");

const protocol = "mqtt";
const host = "10.15.112.195";
const coreiotHost = "app.coreiot.io";
const port = "1883";
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`;

const connectUrl = `${protocol}://${host}:${port}`;
const topic = "esp32/telemetry";
const coreIotTopic = "v1/gateway/telemetry";
const token = "0GcAa75mtnZxysgX3a9N"

const client = mqtt.connect(connectUrl, {
    clientId,
});

const coreIotClient = mqtt.connect(`mqtt://${coreiotHost}:1883`, {
    clientId: `coreiot_${Math.random().toString(16).slice(3)}`,
    username: token,
});

client.on("connect", () => {
    console.log("Connected");

    client.subscribe([topic], () => {
        console.log(`Subscribe to topic '${topic}'`);
    });
});

coreIotClient.on("connect", () => {
    console.log("Connected to Core IOT Gateway");

    coreIotClient.subscribe([coreIotTopic], () => {
        console.log(`Subscribe to Core IOT topic '${coreIotTopic}'`);
    });
});

// client.on("message", (topic, payload) => {
//     payload = JSON.parse(payload.toString());
//     console.log(payload);
//     console.log("Temperature received:", payload.temperature);
//     console.log("Humidity received:", payload.humidity);

//     let telemetry = {
//         "ESP32_001": [
//             {"ts": Date.now(), "values": {"temperature": payload.temperature, "humidity": payload.humidity}}
//         ]
//     }

//     console.log("Publishing to Core IOT Gateway:", telemetry.ESP32_001[0].values);


//     coreIotClient.publish(coreIotTopic, JSON.stringify(telemetry), {qos: 1, retain: false}, (error) => {
//         if (error) {
//             console.error("Publish error:", error);
//         }
//     });
// });

coreIotClient.on("message", (topic, payload) => {
    console.log("Message from Core IOT Gateway:", topic, payload.toString());

    coreIotClient.publish(coreIotTopic, payload, {qos: 1, retain: false}, (error) => {
        if (error) {
            console.error("Publish error:", error);
        }
    });
});

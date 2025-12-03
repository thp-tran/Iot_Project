// localClient.js
const mqtt = require("mqtt");
require("dotenv").config();

const protocol = process.env.PROTOCOL;
const host = process.env.HOST;
const port = process.env.PORT;
const localData = process.env.LOCAL_DATA;
const localAttributeTopic = process.env.LOCAL_ATTRIBUTE_TOPIC

const clientId = `mqtt_${Math.random().toString(16).slice(3)}`;
const connectUrl = `${protocol}://${host}:${port}`;

const client = mqtt.connect(connectUrl, { clientId: clientId });

client.subscribe([localData, localAttributeTopic], () => {
    console.log(`Subscribed to local topic '${localAttributeTopic}'`);
});

client.on("connect", () => {
    console.log("Connected to local broker");
});

module.exports = client;

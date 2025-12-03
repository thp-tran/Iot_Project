const mqtt = require("mqtt");
require("dotenv").config();

const protocol = process.env.PROTOCOL;
const coreiotHost = process.env.COREIOT_HOST;
const port = process.env.PORT;
const coreiotRPC = process.env.COREIOT_RPC;
const token = process.env.TOKEN;

const connectUrl = `${protocol}://${coreiotHost}:${port}`;

const coreIotClient = mqtt.connect(connectUrl, {
    clientId: `coreiot_${Math.random().toString(16).slice(3)}`,
    username: token
});

coreIotClient.subscribe([coreiotRPC], () => {
    console.log(`Subscribed to Core IOT topic '${coreiotRPC}'`);
});
coreIotClient.on("connect", () => {
    console.log("Connected to Core IOT Gateway");
});

module.exports = coreIotClient;

require("dotenv").config();

const localData = process.env.LOCAL_DATA;
const localAttributeTopic = process.env.LOCAL_ATTRIBUTE_TOPIC;
const coreiotAttributeTopic = process.env.COREIOT_ATTRIBUTE_TOPIC;
const coreiotRPC = process.env.COREIOT_RPC;
const coreiotData = process.env.COREIOT_DATA;
const localRPC = process.env.LOCAL_RPC;

const deviceName = process.env.DEVICE_NAME;

function attachHandlers(localClient, coreIotClient) {
  // ESP32 / local → CoreIOT
  localClient.on("message", (topic, payload) => {
    const data = JSON.parse(payload.toString());
    console.log(`Message from ${deviceName}:`, topic, data);

    switch (topic) {
      case localData:
        const payload = {
          temperature: data.temperature,
          humidity: data.humidity,
        };

        coreIotClient.publish(
          coreiotData,
          JSON.stringify(payload),
          { qos: 1, retain: false },
          (error) => {
            if (error) {
              console.error("Publish error:", error);
            }
          }
        );
        break;

      case localAttributeTopic:
        coreIotClient.publish(
          coreiotAttributeTopic,
          JSON.stringify(data),
          { qos: 1, retain: false },
          (error) => {
            if (error) {
              console.error("Publish error:", error);
            }
          }
        );
        break;

      default:
        console.log("Default");
        break;
    }
  });

  // CoreIOT → (hiện tại bạn chỉ log và publish lại chính nó)
  coreIotClient.on("message", (topic, payload) => {
    let newTopic = topic.replace(/\d+$/, "+");

    switch (newTopic) {
      case coreiotRPC:
        console.log(
          "Message from Core IOT Gateway:",
          topic,
          payload.toString()
        );

        coreIotClient.publish(
          topic,
          payload.toString(),
          { qos: 1, retain: false },
          (error) => {
            if (error) {
              console.error("Publish error:", error);
            }
          }
        );

        localClient.publish(
          localRPC,
          payload.toString(),
          { qos: 1, retain: false },
          (error) => {
            if (error) {
              console.error("Publish error:", error);
            }
          }
        );
        break;

      default:
        console.log(payload.toString());
        console.log("Topic not handled:", topic);
        break;
    }
  });
}

module.exports = attachHandlers;

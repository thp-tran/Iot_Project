// index.js
const localClient = require("./gateway/localClient");
const coreIotClient = require("./gateway/coreIotClient");
const attachHandlers = require("./gateway/handlers");

attachHandlers(localClient, coreIotClient);

console.log("Gateway started");

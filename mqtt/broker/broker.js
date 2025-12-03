require("dotenv").config();

const aedes = require("aedes")();
const net = require("net");

const mqttServer = net.createServer(aedes.handle);

const host = process.env.HOST || "0.0.0.0";
const port = process.env.PORT || 1883;

mqttServer.on("connection", (socket) => {
    const clientAddress = `${socket.remoteAddress}:${socket.remotePort}`;
    console.log(`Khách hàng MQTT kết nối: ${clientAddress}`);
});

mqttServer.listen(port, "0.0.0.0", () => {
    console.log(`MQTT Broker đang chạy trên ${host}:${port}`);
});

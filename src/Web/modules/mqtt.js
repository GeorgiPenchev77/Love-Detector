const mqtt = require("mqtt");

const protocol = "mqtt";
const host = "localhost";
const port = "1883";
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`;

const connectURL = `${protocol}://${host}:${port}`;

const client = mqtt.connect(connectURL, {
  clientId,
  clean: true,
  connectTimeout: 4000,
  reconnectPeriod: 1000,
});

//todo: change this
const topics = [
  "start_button_click",
  "stop_button_click",
  "change_question",
  "heart_rate_left",
  "heart_rate_right",
  "match_result",
  "test",
];

client.on("connect", () => {
  console.log("Connected");
  client.subscribe(topics, () => {
    console.log(`Subscribe to topics: '${topics.join(", ")}'`);
  });
});

module.exports = {MQTTclient: client, topics};
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
  "start_button_click",  //0
  "stop_button_click",   //1
  "change_question",     //2
  "heart_rate_left",     //3
  "heart_rate_right",    //4
  "match_result",        //5
  "date_started",        //6
  "heart_rate_both",     //7
  "date_stopped"         //8
];

client.on("connect", () => {
  console.log("Connected");
  client.subscribe(topics, () => {
    console.log(`Subscribe to topics: '${topics.join(", ")}'`);
  });
});

module.exports = {MQTTclient: client, topics};
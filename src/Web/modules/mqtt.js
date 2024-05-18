const mqtt = require("mqtt");

const PROTOCOL = "mqtt";
const HOST = "localhost";
const PORT = "1883";
let clientId = `mqtt_${Math.random().toString(16).slice(3)}`;

let connectURL = `${PROTOCOL}://${HOST}:${PORT}`;

const client = mqtt.connect(connectURL, {
  clientId,
  clean: true,
  connectTimeout: 4000,
  reconnectPeriod: 1000,
});

//Topics being subscribed to from MQTT broker
const TOPIC_TITLES = [
  "start_button_click",  //0
  "stop_button_click",   //1
  "change_question",     //2
  "heart_rate_left",     //3
  "heart_rate_right",    //4
  "match_result",        //5
  "date_started",        //6
  "heart_rate_both",     //7
  "date_stopped",        //8
  "im_started",          //9
  "im_stopped",          //10
  "im_user_switched"     //11
];

let topics = Object.fromEntries(
    TOPIC_TITLES.map((topic) => [topic, topic])
);



client.on("connect", () => {
  console.log("Connected");
  client.subscribe(TOPIC_TITLES, () => {
    console.log(`Subscribe to topics: '${TOPIC_TITLES.join(", ")}'`);
  });
});

module.exports = {MQTTclient: client, topics: Object.freeze(topics)};
const express = require("express");
const app = express();

const { Server } = require("socket.io");
const http = require("http");
const server = http.createServer(app);
const io = new Server(server);

const { compCalc, calcNormalHeartrate } = require("./modules/compatibility.js");
const { MQTTclient, topics } = require("./modules/mqtt.js");
const util = require("./modules/util.js");

app.use(express.static("public/html")); // Serve static files from the 'public' directory
app.use(express.static("public/assets")); // serve questions from assets folder
app.use(express.static("public/css"));
app.use(express.static("public"));

// Start the server
const PORT = process.env.PORT || 3000;

const TEMPLATE_FILE = "heartbeatData.json";
const UPDATE_FILE = "newHeartbeatData.json";

let leftArray = [];
let rightArray = [];

let isDateStarted = false;
let isIMStarted = false;

let dateTimer; //variable for interval
const DATE_DURATION = 180000; // = 3 min. Duration of the date
const hbRequests = 10; // number of heartbeat scans during the date
const imHbRequests = 5; //number of heartbeat scans for individual measurement

MQTTclient.on("message", (topic, payload) => {
  if (topics[0] == topic) {
    io.emit("start");
  } else if (topics[1] == topic) {
    io.emit("stop");
  } else if (topics[2] == topic) {
    io.emit("next_question");
  } else if (topics[3] == topic) {
    processHeartbeat(0, parseInt(payload));
  } else if (topics[4] == topic) {
    processHeartbeat(1, parseInt(payload));
  } else if (topics[7] == topic) {
    processBothHeartbeats(payload.toString());
  }

  console.log("Received message:", topic, payload.toString());
});

function processHeartbeat(id, measure) {
  if(!isIMStarted){
    throw new Error("Individual hearbeat received, but individual measurement has not started");
  }

  let hbArray;
  if (id == 0) {
    hbArray = leftArray;
  } else if (id == 1) {
    hbArray = rightArray;
  }

  if (hbArray.length <= 4) {
    hbArray.push(measure);
    io.emit("progress");
  }
  if (hbArray.length == imHbRequests) {
    let user0normal = calcNormalHeartrate(hbArray);
    saveIndividualMeasurement(id, user0normal);
    hbArray.length = 0;
  }
}

function processBothHeartbeats(measure){
  if(!isDateStarted){
    throw new Error("Both heartbeats received, but the date has not started");
  }

  const stringArray = measure.split(' ');
  const heartBeatArray = stringArray.map((x) => parseInt(x));
  if(Number.isInteger(heartBeatArray[1]) && Number.isInteger(heartBeatArray[1])){
  leftArray.push(heartBeatArray[0]);
  rightArray.push(heartBeatArray[1]);
  }
  console.log(leftArray.length);
  console.log("Both HBs received: left: " + heartBeatArray[0] + ", right: " + heartBeatArray[1]);

  if(leftArray.length == hbRequests){
    endDate();
  }
}

function goToResult(){
  io.emit("endDate");
}

io.on("connection", (socket) => {
  
  socket.on("startIM", () => {
    isIMStarted = true;
    MQTTclient.publish(topics[9], parseInt(imHbRequests) + "");
    console.log("IM started");
  });
  socket.on('changeCurrentUser', () => {
    MQTTclient.publish(topics[11], "1");
    console.log("User Changed");
  });
  socket.on('endIM', () => {
    isIMStarted = false;
    MQTTclient.publish(topics[10], "1");
    console.log("IM ended");
  });
  socket.on("dateStarted", () => {
    socket.join("dateRoom");
    isDateStarted = true;
    MQTTclient.publish(topics[6], parseInt(DATE_DURATION / hbRequests) + "");
    console.log("Date started");
  });
  socket.on("disconnecting", () => {
    if (socket.rooms.has("dateRoom")) {
      isDateStarted = false;
      MQTTclient.publish(topics[8], "1");
    }
  });
  socket.on("resetIM", ()=>{
    resetHBdata();
  });

  console.log("A user connected");
});

function resetHBdata(){
  leftArray = [];
  rightArray = [];
}

function endDate() {
  saveDateMeasurements();
  compCalc();
  activateLED();
  goToResult();
  console.log("Date has ended");
}

function saveIndividualMeasurement(id, heartbeat) {
  util.updateJSON(UPDATE_FILE, (existingData) => {
    existingData.users[id].normal_heartbeat = heartbeat;
  });
}

function saveDateMeasurements() {
  util.updateJSON(UPDATE_FILE, (existingData) => {
    console.log(leftArray);
    console.log(rightArray);
    existingData.users[0].heartbeat_data = leftArray;
    existingData.users[1].heartbeat_data = rightArray;
  });
}

function activateLED() {
  util.readJSON(UPDATE_FILE, (data) => {
    let result = data.match_result + "";
    MQTTclient.publish(topics[5], result);
  });
}

server.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});

app.use(express.json());

app.post("/saveUserData", (req, res) => {
  const userData = req.body;

  // Read the existing JSON file

  util.readJSON(TEMPLATE_FILE, (existingData) => {
    //Update the username and pronouns for the newly entered users.
    existingData.users[0].username = userData.user1.username || "";
    existingData.users[0].pronouns = userData.user1.pronouns || "";
    existingData.users[1].username = userData.user2.username || "";
    existingData.users[1].pronouns = userData.user2.pronouns || "";

    const jsonData = JSON.stringify(existingData, null, 2);
    //Save the updated info to the json file.
    util.saveJSON(UPDATE_FILE, jsonData);

    console.log("User data saved successfully.");
    res.json({ message: "User data saved successfully." });
  });
});

app.get("/getUserData", (req, res) => {
  util.readJSON(UPDATE_FILE, (existingData) => {
    res.json(existingData);
  });
});

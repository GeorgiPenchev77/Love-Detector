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
let isLeftIMStarted = false;
let isRightIMStarted = false;

let dateTimer; //variable for interval
const DATE_DURATION = 180000; // = 3 min. Duration of the date
const hbRequests = 10; // number of heartbeat scans during the date

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
    processBothHeartbeats(payload);
  }

  console.log("Received message:", topic, payload.toString());
});

function processHeartbeat(id, measure) {
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
  if (hbArray.length === 5) {
    let user0normal = calcNormalHeartrate(hbArray);
    saveIndividualMeasurement(id, user0normal);
    hbArray.length = 0;
  }
}

function processBothHeartbeats(payload){
  if(!isDateStarted){
    throw new Error("Both heartbeats received, but the date has not started");
  }


  let [leftHB, rightHB] = payload.split(' ').map((x) => {parseInt(x)});
  leftArray.push(leftHB);
  rightArray.push(rightHB);

  console.log("Both HBs received: left: " + leftHB + ", right: " + rightHB);

  if(leftArray.length() == hbRequests){
    goToResult();
  }

}


function goToResult(){
  io.emit("endDate");
}

io.on("connection", (socket) => {
  socket.on("dateStarted", () => {
    socket.join("date-room");
    isDateStarted = true;
    MQTTclient.publish(topics[6], parseInt(DATE_DURATION / hbRequests) + "");
    console.log("Date started");
  });
  socket.on("endDate", () => {
    endDate();
  });
  socket.on("disconnecting", () => {
    if (socket.rooms.has("dateRoom")) {
      isDateStarted = false;
      resetHBdata();
      MQTTclient.publish(topics[8], "1");
    }
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
  console.log("date has ended");
}

function saveIndividualMeasurement(id, heartbeat) {
  util.updateJSON(UPDATE_FILE, (existingData) => {
    existingData.users[id].normal_heartbeat = heartbeat;
  });
}

function saveDateMeasurements() {
  util.updateJSON(UPDATE_FILE, (existingData) => {
    existingData.users[0].heartbeat_data = leftArray;
    existingData.users[1].heartbeat_data = rightArray;
  });
  resetHBdata();
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

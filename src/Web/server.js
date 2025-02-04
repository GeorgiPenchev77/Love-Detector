const express = require("express");
const app = express();

const { Server } = require("socket.io");
const http = require("http");
const server = http.createServer(app);
const io = new Server(server);

const { compCalc, individualMeasurementCalc, DATE_DURATION, NUMBER_OF_MEASUREMENTS } = require("./modules/compatibility.js");
const { MQTTclient, topics } = require("./modules/mqtt.js");
const util = require("./modules/util.js");

app.use(express.static("public/html")); // Serve static files from the 'public' directory
app.use(express.static("public/assets")); // serve questions from assets folder
app.use(express.static("public/css"));
app.use(express.static("public"));

// start the server
const PORT = process.env.PORT || 3000;

const TEMPLATE_FILE = "heartbeatData.json";
const UPDATE_FILE = "newHeartbeatData.json";

let leftArray = [];
let rightArray = [];

let isDateStarted = false;
let isIMStarted = false;

let dateTimer; //variable for interval
const ImHbRequests = 5; //number of heartbeat scans for individual measurement

//MQTT message handler
MQTTclient.on("message", (topic, payload) => {
  if (topic === topics.start_button_click) {
    io.emit("start");
  } else if (topic === topics.stop_button_click) {
    io.emit("stop");
  } else if (topic === topics.change_question) {
    io.emit("next_question");
  } else if (topic === topics.heart_rate_left) {
    processHeartbeat(0, parseInt(payload));
  } else if (topic === topics.heart_rate_right) {
    processHeartbeat(1, parseInt(payload));
  } else if (topic === topics.heart_rate_both) {
    processBothHeartbeats(payload.toString());
  }

  console.log("Received message:", topic, payload.toString());
});


/*Process individual heartbeat.
A callback function to process heartbeats received during Individual Measurements*/
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

  if (hbArray.length < ImHbRequests) {
    hbArray.push(measure);
    io.emit("progress");
  }
  if (hbArray.length == ImHbRequests) {
    saveIndividualMeasurement(id, hbArray);
    individualMeasurementCalc(id);
    hbArray.length = 0;
  }
}

// Process both users heartbeat data received during the date
function processBothHeartbeats(measure){
  if(!isDateStarted){
    throw new Error("Both heartbeats received, but the date has not started");
  }

  const stringArray = measure.split(' ');

  const heartBeatArray = stringArray.map((x) => parseInt(x));
  if(Number.isInteger(heartBeatArray[0]) && Number.isInteger(heartBeatArray[1])){
  leftArray.push(heartBeatArray[0]);
  rightArray.push(heartBeatArray[1]);
  }
  else{
  throw new Error("Invalid data.");
  }
  console.log("Both HBs received: left: " + heartBeatArray[0] + ", right: " + heartBeatArray[1]);

  if(leftArray.length == NUMBER_OF_MEASUREMENTS){
    endDate(); // End the date if enough heartbeat data has been received
  }
}


// Signal the webpage to proceed to result screen
function goToResult(){
  io.emit("endDate");
}

// Socket.IO connection handler
io.on("connection", (socket) => {
  socket.on("startIM", () => {
    isIMStarted = true;
    MQTTclient.publish(topics.im_started, parseInt(ImHbRequests) + ""); // Notify the start of individual measurement
    console.log("IM started");
  });
  socket.on('changeCurrentUser', () => {
    MQTTclient.publish(topics.im_user_switched, "1"); //Notify user change
    console.log("User Changed");
  });
  socket.on('endIM', () => {
    isIMStarted = false;
    MQTTclient.publish(topics.im_stopped, "1"); //Notify end of individual measurement
    console.log("IM ended");
  });
  socket.on("dateStarted", () => {
    socket.join("dateRoom");
    isDateStarted = true;
    MQTTclient.publish(topics.date_started, parseInt(DATE_DURATION / NUMBER_OF_MEASUREMENTS) + ""); //Notify start of the actual date
    console.log("Date started");
  });
  socket.on("disconnecting", () => {
    if (socket.rooms.has("dateRoom")) {
      isDateStarted = false;
      MQTTclient.publish(topics.date_stopped, "1"); // Notify the end of the date on disconnect
    }
  });
  socket.on("resetIM", ()=>{
    resetHBdata();
  });

  console.log("A user connected");
});

// Reset heartbeat data in the arrays after measurement is done
function resetHBdata(){
  leftArray = [];
  rightArray = [];
}

// Process the collected data upon ending the date
function endDate() {
  saveDateMeasurements(); 
  compCalc(); 
  activateLED();
  goToResult();
  console.log("Date has ended");
}

// Save individual measurements to JSON file
function saveIndividualMeasurement(id, heartbeatData) {
  util.updateJSON(UPDATE_FILE, (existingData) => {
    existingData.users[id].IM_heartbeat_data = heartbeatData;
  });
}

// Save date measurements to JSON file
function saveDateMeasurements() {
  util.updateJSON(UPDATE_FILE, (existingData) => {
    existingData.users[0].date_heartbeat_data = leftArray;
    existingData.users[1].date_heartbeat_data = rightArray;
  });
}

// Activate LED based on the match result
function activateLED() {
  util.readJSON(UPDATE_FILE, (data) => {
    let result = data.match_result + "";
    MQTTclient.publish(topics.match_result, result);
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

//Endpoint to get user data from JSON
app.get("/getUserData", (req, res) => {
  util.readJSON(UPDATE_FILE, (existingData) => {
    res.json(existingData);
  });
});

const express = require("express");
const app = express();

const { Server } = require("socket.io");
const http = require("http");
const server = http.createServer(app);
const io = new Server(server);
const mqtt = require('mqtt');
const fs = require('fs');

const {compCalc, calcNormalHeartrate } = require('./compatibility.js')

app.use(express.static("public/html")); // Serve static files from the 'public' directory
app.use(express.static("public/assets")); // serve questions from assets folder
app.use(express.static("public/css"));
app.use(express.static("public"));

// Start the server
const PORT = process.env.PORT || 3000;

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
const topics = ['start_button_click', 'stop_button_click', 'change_question', 'heart_rate_left', 'heart_rate_right'];

client.on("connect", () => {
  console.log("Connected");
  client.subscribe(topics, () => {
    console.log(`Subscribe to topics: '${topics.join(", ")}'`);
  });
});

let leftArray = [];
let rightArray = [];

let isDateStarted = false;
let isLeftIMStarted = false;
let isRightIMStarted = false;

function writeToJSON(id, value, valueType){
  fs.readFile("newHeartbeatData.json", (err, data) => {
    if (err) {
        console.error("Failed to read JSON file:", err);
        return res.status(500).json({ error: "Failed to read JSON file." });
    }

    let existingData = JSON.parse(data);

    if(valueType == "normal_heartbeat"){
      existingData.users[id].normal_heartbeat = value || "";
    } else if(valueType == "heartbeat_data"){
      existingData.users[0].heartbeat_data = leftArray;
      existingData.users[1].heartbeat_data = rightArray;
      existingData.test_data_for_graph.time_seconds = value;
      leftArray = [];
      rightArray = [];
    }
    
    
  const jsonData = JSON.stringify(existingData, null, 2);

  //Save the updated info to the json file.
  fs.writeFile("newHeartbeatData.json", jsonData, (err) => {
      if (err) {
          console.error("Failed to save user data:", err);
          return res.status(500).json({ error: "Failed to save user data." });
      }
      console.log("User data saved successfully.");
  });
});
}



client.on("message", (topic, payload) => {
  if (topics[0] == topic) {
    io.emit('start');
  }
  else if(topics[1] == topic){
    io.emit('stop');
  }
  else if (topics[2] == topic){
    io.emit('next_question');
  }
  else if (topics[3] == topic){
    processHeartbeat(0, parseInt(payload))
  }
  else if (topics[4] == topic){
    processHeartbeat(1, parseInt(payload))
  }

  console.log('Received message:', topic, payload.toString());
});

function processHeartbeat(id, measure){

  let hbArray;
  if(id == 0){
    hbArray = leftArray;
  } else if (id == 1){
    hbArray = rightArray;
  }

  if(isDateStarted){
    hbArray.push(measure);
  } else {

    if(hbArray.length<=4){
      hbArray.push(measure);
      io.emit('progress');
    }
    if(hbArray.length===5){
      let user0normal=calcNormalHeartrate(hbArray);
      writeToJSON(id,user0normal, "normal_heartbeat");
      hbArray.length = 0;
    }
  }
}



io.on('connection', (socket) => {
  socket.on('dateStarted', () => {
    socket.join("date-room");
    isDateStarted = true;
    console.log('Date started');
  })
  socket.on('generateResult', (time) => {
    writeToJSON(null, time, "heartbeat_data");
    compCalc();
    console.log("dateEnded");
  })
  socket.on('disconnecting', () => {
    if(socket.rooms.has('dateRoom')){
      isDateStarted = false;
    }
  })
  console.log("A user connected");
});



server.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});


app.use(express.json());

app.post("/saveUserData", (req, res) => {
    const userData = req.body;


    // Read the existing JSON file
    fs.readFile("heartbeatData.json", (err, data) => {
      if (err) {
          console.error("Failed to read JSON file:", err);
          return res.status(500).json({ error: "Failed to read JSON file." });
      }

      let existingData = JSON.parse(data);

      //Update the username and pronouns for the newly entered users. 
      existingData.users[0].username = userData.user1.username || "";
      existingData.users[0].pronouns = userData.user1.pronouns || "";
      existingData.users[1].username = userData.user2.username || "";
      existingData.users[1].pronouns = userData.user2.pronouns || "";
      
      

    const jsonData = JSON.stringify(existingData, null, 2);

    //Save the updated info to the json file.
    fs.writeFile("newHeartbeatData.json", jsonData, (err) => {
        if (err) {
            console.error("Failed to save user data:", err);
            return res.status(500).json({ error: "Failed to save user data." });
        }
        console.log("User data saved successfully.");
        res.json({ message: "User data saved successfully." });
    });
  });
});

app.get("/getUserData", (req, res) => {
  // Read the existing JSON file
  fs.readFile("newHeartbeatData.json", (err, data) => {
    if (err) {
        console.error("Failed to read JSON file:", err);
        return res.status(500).json({ error: "Failed to read JSON file." });
    }

    let existingData = JSON.parse(data);
    res.json(existingData);
  });
});


/*current state:
 - individual heartbeat: the data from one user interferes with the measure of the other.
    Possible solution: track if the data incomign in processHeartbeat() is for the correct user.
        Also, check if 2 heartbeats are sent at the same time on the wio side
        
 - date heartbeat: unknown. Possibly working, but in the middle of debugging the bug mention earlier was found
    During the last test the saving saved was completely incorrect(but it was changed after and not yet tested)

  Good luck champion. 


*/

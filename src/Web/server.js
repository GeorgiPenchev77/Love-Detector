const express = require("express");
const app = express();

const { Server } = require("socket.io");
const http = require("http");
const server = http.createServer(app);
const io = new Server(server);
const mqtt = require('mqtt');
const fs = require('fs');

const compCalc = require('./compatibility.js')

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

const topics = ['start_button_click', 'stop_button_click', 'change_question', 'heart_rate_left', 'heart_rate_right'];

client.on("connect", () => {
  console.log("Connected");
  client.subscribe(topics, () => {
    console.log(`Subscribe to topics: '${topics.join(", ")}'`);
  });
});

client.on("message", (topic, payload) => {
  if (topics[0] == topic) {
    io.emit('start');
  }
  else if(topics[1] == topic){
    io.emit('stop');
  }
  else if (topics[2] == topic) {
    io.emit('next_question');
  }
  else if (topics[3] == topic) {
    io.emit('heartRateLeft');
  }
  else if (topics[4] == topic){
    io.emit('hearRateRight');
  }

  console.log('Received message:', topic, payload.toString());
});


io.on('connection', (socket) => {
  console.log("A user connected");
});

server.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
  compCalc();
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

const express = require('express');
const app = express();

const { Server } = require('socket.io');
const http = require('http');
const server = http.createServer(app);
const io = new Server(server);
const mqtt = require('mqtt');
const fs = require('fs');


app.use(express.static('public')); // Serve static files from the 'public' directory
app.use(express.static('public/assets')); // serve questions from assets folder
// Start the server
const PORT = process.env.PORT || 3000;




const protocol = 'mqtt';
const host = 'localhost';
const port = '1883';
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`;

const connectURL = `${protocol}://${host}:${port}`;

const client = mqtt.connect(connectURL, {
  clientId,
  clean: true,
  connectTimeout: 4000,
  reconnectPeriod: 1000,
})

const topics = ['startbutton_click', 'individualMeasure_button', 'change_question'];

client.on('connect', () => {
  console.log('Connected');
  client.subscribe(topics, () => {
    console.log(`Subscribe to topics: '${topics.join(', ')}'`)
  });
});


client.on("message", (topic, payload) => {
  if (topics[0] == topic){
    io.emit('switchpage', { nextPage: './questions.html' });
  }
  else if(topics[1] == topic){
    io.emit('measuringMessage'); 
  }else{
    io.emit('next_question');
    console.log("1");
  }
  console.log('Received message:', topic, payload.toString());
});

io.on('connection', (socket) => {
  console.log("A user connected");
});

server.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});

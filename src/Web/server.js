const express = require('express');
const app = express();

const { Server } = require('socket.io');
const http = require('http');
const server = http.createServer(app);
const io = new Server(server);
const mqtt = require('mqtt');
const fs = require('fs');


app.use(express.static('public/html')); // Serve static files from the 'public' directory
app.use(express.static('public/assets')); // serve questions from assets folder
app.use(express.static('public/css'));
app.use(express.static('public'));

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

const topics = ['start_button_click', 'individual_measure_button_click', 'change_question', 'heart_rate', 'test'];

client.on('connect', () => {
  console.log('Connected');
  client.subscribe(topics, () => {
    console.log(`Subscribe to topics: '${topics.join(', ')}'`)
  });
});


client.on("message", (topic, payload) => {
  
  if (topics[0] == topic) {
    io.emit('switchpage', { nextPage: './questions.html' });
  }
  else if(topics[1] == topic){
    io.emit('measuringMessage'); 
  }
  else if (topics[2] == topic) {
    io.emit('next_question');
  }

  console.log('Received message:', topic, payload.toString());
});

io.on('connection', (socket) => {
  console.log("A user connected");
});

server.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});

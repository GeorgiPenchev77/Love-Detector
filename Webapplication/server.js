const express = require('express');
const app = express();

const { Server } = require('socket.io');
const http = require('http');
const server = http.createServer(app);
const io = new Server(server);
const mqtt = require('mqtt');


app.use(express.static('public')); // Serve static files from the 'public' directory

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

const topic = 'startbutton_click';

client.on('connect', () => {
  console.log('Connected');
  client.subscribe([topic], () => {
    console.log(`Subscribe to topic '${topic}'`)
  });
});


client.on("message", (topic, payload) => {
  console.log('Received message:', topic, payload.toString());
});


server.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});

const express = require('express')
const path = require('path');
const app = express()
const port = 3000

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'index.html')); 
})

app.get('/Kristina', (req, res) => {
  console.log("hello")
  res.sendFile(path.join(__dirname, 'measurement.html')); 
})

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
})
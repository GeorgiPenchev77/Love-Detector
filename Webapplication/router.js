const express = require('express');
const path = require('path');
const router = express.Router();

// Define route handler for /heartbeat
router.get('/heartbeat', (req, res) => {
    console.log("Heartbeat received");
    // Send the measurement.html file
    res.sendFile(path.join(__dirname, 'measurement.html'));
});

router.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'index.html'));
});


module.exports = router;

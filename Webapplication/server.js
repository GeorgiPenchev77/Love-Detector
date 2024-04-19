const express = require('express');
const app = express();


app.use(express.static('public')); // Serve static files from the 'public' directory

// Start the server
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});

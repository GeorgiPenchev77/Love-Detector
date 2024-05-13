const fs = require("fs");

const readJSON = (file, process) => {
  fs.readFile(file, (err, data) => {
    if (err) {
      console.error("Failed to read:", err);
      return res.status(500).json({ error: "Failed to read." });
    }

    let newData = JSON.parse(data);

    process(newData);
    
  });
}

const saveJSON = (file, data) => {
  fs.writeFile(file, data, (err) => {
    if (err) {
      console.error("Failed to save user data:", err);
      return res.status(500).json({ error: "Failed to save user data." });
    }
  });
}

const updateJSON = (file, update) => {
  readJSON(file, (data) =>{
    update(data);
    let jsonData = JSON.stringify(data, null, 2);
    saveJSON(file, jsonData);
  })
}

module.exports = {readJSON, updateJSON, saveJSON}
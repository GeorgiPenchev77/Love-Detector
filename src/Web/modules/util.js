const fs = require("fs");

// Read and parse JSON file
const readJSON = (file, process) => {
  let dataJson = fs.readFileSync(file);
  let newData = JSON.parse(dataJson);
  process(newData);
}

// Write data to the JSON file
const saveJSON = (file, data) => {
  fs.writeFileSync(file, data);
}

// Update the JSON file with new data
const updateJSON = (file, update) => {
  try{
    readJSON(file, (data) =>{
      update(data);
      let jsonData = JSON.stringify(data, null, 2);
      saveJSON(file, jsonData);
    })
  }catch (error) {
    console.error("Error updating JSON:", error);
    throw error;
  }
}


// get the average heartbeat from array of gathered heartbeat values
function calcAverage (array) {
  let avg = 0;
  for (let i = 0; i < array.length; i++) {
    avg += array[i];
  }
  return parseInt(avg / array.length);
}

module.exports = {calcAverage, readJSON, updateJSON, saveJSON}
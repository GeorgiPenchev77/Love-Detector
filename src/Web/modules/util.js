const fs = require("fs");

const readJSON = (file, process) => {
  let dataJson = fs.readFileSync(file);
  let newData = JSON.parse(dataJson);
  process(newData);
}

const saveJSON = (file, data) => {
  fs.writeFileSync(file, data);
}

const updateJSON = (file, update) => {
  readJSON(file, (data) =>{
    update(data);
    let jsonData = JSON.stringify(data, null, 2);
    saveJSON(file, jsonData);
  })
}


// get the average from an array of values
function calcAverage (array) {
  let avg = 0;
  for (let i = 0; i < array.length; i++) {
    avg += array[i];
  }
  return parseInt(avg / array.length);
}

module.exports = {calcAverage, readJSON, updateJSON, saveJSON}
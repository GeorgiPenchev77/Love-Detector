const fs = require("fs");

const readJSON = (file, process) => {
  const dataJson = fs.readFileSync(file);
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

module.exports = {readJSON, updateJSON, saveJSON}
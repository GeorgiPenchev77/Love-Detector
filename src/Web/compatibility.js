const fs = require("fs")

const numberOfUsers = 2;

const comparePointsNum = 10;

const RESULT_JSON = "./heartbeatData.json"

const compCalc = () => {
  //generate .json using function below
  try{
    const dataJson = fs.readFileSync(RESULT_JSON);
    data = JSON.parse(dataJson);

    data.users.forEach(user => {
      user.date_heartbeat_avg = dateAvr(user);
      user.date_heartbeat_peak = datePeak(user);
    });

    data.test_data_for_graph.number_of_points = comparePointsNum;
    data.test_data_for_graph.time_seconds = null;

    data.match_result = match(data.users);

    fs.writeFileSync(RESULT_JSON, JSON.stringify(data, null, 2));
    console.log("File updated");
  } catch (error) {

    console.log(error);
    throw error;

  }
  
  

};

const regAvr = (user) => {
  //return regAvr hb of a user
  return null;
};

const dateAvr = (user) => {
  //return dateAvr hb of a user
  return null;
};

const datePeak = (user) => {
  //return peak hb of a user during the date
  return null;
};

const match = (users) => {
  //return compatibility score
  return null;
};


module.exports = compCalc;

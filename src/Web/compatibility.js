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
      user.date_spike_Counter = calculateSpike(user);
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
  const arrayHb = user.heartbeat_data;
  let sum = 0;
  for (let i = 0; i < arrayHb.length; i++) {
    sum = arrayHb[i] + sum;
  }
  const averageHb = sum / arrayHb.length;

  //return dateAvr hb of a user
  return averageHb;
};


const datePeak = (user) => {
  let peak = 0;
  const arrayHb = user.heartbeat_data;
  for (let i=0; i< arrayHb.length; i++){
    if ( peak < arrayHb[i]){
      peak = arrayHb[i];
    }
  }
  return peak ;
};

const calculateSpike = (user) => { // there is no spike in json file
  let spikeCounter=0;
  const arrayHb = user.heartbeat_data;

  for (let i=1; i< arrayHb.length; i++){
    const difference = Math.abs(arrayHb[i] - arrayHb[i - 1]);
    if (difference>=15){
      spikeCounter = spikeCounter+1;
    }
  }
  return spikeCounter;
};

const UpperBracket = (user) =>{
 // indAverageHb
  return null;
};

const indCompositeScore = (user) =>{

  return null;
};

const match = (users) => {


  //return compatibility score
  return null;
};




module.exports = compCalc;

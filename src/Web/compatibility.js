const fs = require("fs")

const numberOfUsers = 2;

const comparePointsNum = 10;

const RESULT_JSON = "./newHeartbeatData.json"
const NEW_RESULT_JSON = "./newHeartbeatData.json"

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
    //data.test_data_for_graph.time_seconds = null;

    data.match_result = match(data.users);

    fs.writeFileSync(NEW_RESULT_JSON, JSON.stringify(data, null, 2));
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
  return calcAvarage(arrayHb);
};

const calcAvarage = (array) => {
  let avg = 0;
  for(let i=0; i<array.length; i++ ){
    avg += array[i];
  }
  return avg/array.length;
}

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




module.exports = {compCalc, calcNormalHeartrate: calcAvarage};

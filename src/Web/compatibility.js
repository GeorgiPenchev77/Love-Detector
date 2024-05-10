const fs = require("fs")

const numberOfUsers = 2;

const comparePointsNum = 10;

const RESULT_JSON = "./heartbeatData.json"

const compCalc = () => {
  //generate .json using function below
  try{
    const dataJson = fs.readFileSync(RESULT_JSON);
    data = JSON.parse(dataJson) ;

    data.users.forEach(user => {
      user.date_heartbeat_avg = dateAvr(user);
      user.date_heartbeat_peak = datePeak(user);
      user.date_spike_counter = calculateSpike(user);
      user.date_upper_bracket = calculateUpperBracket(user);
      user.composite_score = indCompositeScore(user);
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
  let testAverage = 120;
  return testAverage;
};

const regPeak = (user) => {
  let testPeak = 120;
  return testPeak;
}

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

const calculateUpperBracket = (user) =>{
  const indAverageHb= 70;
  const indPeak = 110;
  
  IM= indAverageHb + ((indPeak- indAverageHb)/2);
  return IM;
};

const hearRateVariability = (user) =>{
  //how many spikes with in a measured time
  //60 seconds / spikes+1    //have to avoid it being 0 in the denominator
  // bellow 60 seconds / 1 spike +<1
  const variability = 60 / 1+1;
  return variability;
}

const indCompositeScore = (user) =>{
  const w0 = 0.1;
  const w1  =0;

  let compositeScore =  w0 * (hearRateVariability(user) * 
    (datePeak(user) - (regPeak(user) ) + ( dateAvr(user) - regAvr(user))))+w1;
  return compositeScore;
};

const match = (users) => {


  //return compatibility score
  return null;
};




module.exports = compCalc;

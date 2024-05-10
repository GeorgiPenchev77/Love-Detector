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
      user.calculateUpperBracket = calculateUpperBracket(user);
      
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
  let testAverage = 60;
  return testAverage;
};

const regPeak = (user) => {
  let testPeak = 65;
  return testPeak;
};

const dateAvr = (user) => {
  const arrayHb = user.heartbeat_data;
  let sum = 0;
  for (let i = 0; i < arrayHb.length; i++) {
    sum = arrayHb[i] + sum;
  }
  const averageHb = sum / arrayHb.length;

  //return dateAvr hb of a user
  return averageHb.toFixed(2);
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

const calculateSpike = (user) => { 
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
  const indAverageHb= regAvr(user);
  const indPeak = regPeak(user);
  
  let upperBracket= indAverageHb + ((indPeak- indAverageHb)/2);
  return upperBracket;
};

const counterUpperBracket = (user) =>{
  const upperBracket = calculateUpperBracket(user);
  let aboveUpperBracket = 0;
  const arrayHb = user.heartbeat_data;
  for (let i = 0; i < arrayHb.length; i++) {
    if (arrayHb[i] > upperBracket) {
      aboveUpperBracket++;
    }
  }
  return aboveUpperBracket;
};

const indCompositeScore = (user) => {
  const maxDiff = 60; // normal heartbeat is between 60-100 bpm, im accounting for a bit more
  const TRHIndex = counterUpperBracket(user);

  let normalizedAvgDiff = Math.abs(dateAvr(user) - regAvr(user)) / maxDiff;
  let normalizedPeakDiff = Math.abs(datePeak(user) - regPeak(user)) / maxDiff;

  let compositeScore = Math.abs( ((calculateSpike(user)/30) * (normalizedPeakDiff - normalizedAvgDiff))+ (TRHIndex/30))*10; //30 because amount of measurments

  return compositeScore.toFixed(2);
};

const match = (users) => {

  const user1 = users[0];//4,58
  const user2 = users[1];//12,36

  if (user1.composite_score>3.75 && user2.composite_score>3.75){
    console.log("level 2");
  }
  else if (user1.composite_score>10.5 && user2.composite_score>10.5){
    console.log("level 3");
  }
  else console.log("level 1");
};

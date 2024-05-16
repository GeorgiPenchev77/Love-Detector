const fs = require("fs")

const numberOfUsers = 2;

const comparePointsNum = 10;

//File with initial data for the calculations
const RESULT_JSON = "./newHeartbeatData.json"

//File to write the calculations in
const NEW_RESULT_JSON = "./newHeartbeatData.json"

const individualMeasuementCalc = (userID) => {
  
  //generate .json using function below
  try{
    const dataJson = fs.readFileSync(RESULT_JSON);
    data = JSON.parse(dataJson) ;
      
      data.users[userID].IM_average_heartbeat = testAvr(data.users[userID]);
      data.users[userID].IM_heartbeat_peak = testPeak(data.users[userID]);
      data.users[userID].IM_upper_bracket = calculateIMUpperBracket(data.users[userID]);
      
    fs.writeFileSync(NEW_RESULT_JSON, JSON.stringify(data, null, 2));
    console.log("File updated");
  } catch (error) {

    console.log(error);
    throw error;

  }
};

const compCalc = () => {
  //generate .json using function below
  try{
    const dataJson = fs.readFileSync(RESULT_JSON);
    data = JSON.parse(dataJson) ;

    data.users.forEach(user => {
      
      user.date_average_heartbeat = dateAvr(user);
      user.date_heartbeat_peak = datePeak(user);
      user.date_upper_bracket = calculateDateUpperBracket(user);
      user.date_spike_counter = calculateSpike(user);
      user.composite_score = indCompositeScore(user);
      
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

const calcAvarage = (array) => {
  let avg = 0;
  for(let i=0; i<array.length; i++ ){
    avg += array[i];
  }
  return parseInt(avg/array.length);
}

const testAvr = (user) => {//placeholder function to calculate average of test before date
  const arrayHb = user.IM_heartbeat_data;
  return calcAvarage(arrayHb);
};

const dateAvr = (user) => { // calculates average value between all elements in array 
  const arrayHb = user.date_heartbeat_data;
  return calcAvarage(arrayHb);
};

const testPeak = (user) => { //placeholder function to record peak of test before date
  let peak = 0;
  const arrayHb = user.IM_heartbeat_data;
  for (let i=0; i< arrayHb.length; i++){
    if ( peak < arrayHb[i]){
      peak = arrayHb[i];
    }
  }
  return peak ;
};

const datePeak = (user) => { // saves the highest elements in array
  let peak = 0;
  const arrayHb = user.date_heartbeat_data;
  for (let i=0; i< arrayHb.length; i++){
    if ( peak < arrayHb[i]){
      peak = arrayHb[i];
    }
  }
  return peak ;
};

const calculateIMUpperBracket = (user) =>{  //calculates where the upperBracket should be to facilitate later calculations
  const indAverageHb= testAvr(user);
  const indPeak = testPeak(user);
  
  let upperBracket= indAverageHb + ((indPeak- indAverageHb)/2);
  return parseInt(upperBracket);
};

const calculateDateUpperBracket = (user) =>{  //calculates where the upperBracket should be to facilitate later calculations
  const indAverageHb= dateAvr(user);
  const indPeak = datePeak(user);
  
  let upperBracket= indAverageHb + ((indPeak- indAverageHb)/2);
  return parseInt(upperBracket);
};

const calculateSpike = (user) => { // calculates how many times there is a difference of 15 or greater between elements in array
  let spikeCounter=0;
  const arrayHb = user.date_heartbeat_data;

  for (let i=1; i< arrayHb.length; i++){
    const difference = Math.abs(arrayHb[i] - arrayHb[i - 1]);
    if (difference>=15){
      spikeCounter = spikeCounter+1;
    }
  }
  return spikeCounter;
};

const counterUpperBracket = (user) =>{ // counts how many elements in the array are above the upperBracket
  const upperBracket = calculateDateUpperBracket(user);
  let aboveUpperBracket = 0;
  const arrayHb = user.heartbeat_data;
  for (let i = 0; i < arrayHb.length; i++) {
    if (arrayHb[i] > upperBracket) {
      aboveUpperBracket++;
    }
  }
  return aboveUpperBracket;
};

const indCompositeScore = (user) => { //takes all functions and calculates a score roughly between 0-15 (needs more testing)
  const maxDiff = 60; // normal heartbeat is between 60-100 bpm, im accounting for a bit more
  const TRHIndex = counterUpperBracket(user);

  let normalizedAvgDiff = Math.abs(dateAvr(user) - regAvr(user)) / maxDiff;
  let normalizedPeakDiff = Math.abs(datePeak(user) - regPeak(user)) / maxDiff;

  let compositeScore = Math.abs( ((calculateSpike(user)/30) * (normalizedPeakDiff - normalizedAvgDiff))+ (TRHIndex/30))*10; //30 because amount of measurments

  return parseInt(compositeScore);
};

const match = (users) => {  // calculates whether or not both values are above a threshhold and if they are far apart,
  let level =0;             // this sees is they are match
  const user1 = users[0];   // current value 4,58
  const user2 = users[1];   // current value 12,36
  const diffCompScore = user1 - user2;
  
  const limitLevel3= 15*0.75;
  const limitLevel2 = 15*0.25;
  const diffLevel3 = 3;
  const diffLevel2 = 5;

  if (user1.composite_score > limitLevel3 && user2.composite_score > limitLevel3 || diffCompScore <= diffLevel3 && user1.composite_score > limitLevel3  // 3 cases where it can be level 3
    ||diffCompScore <= diffLevel3 && user2.composite_score > limitLevel3){                                              // all above threshhold, 
    level = 3;                                                                                          //or one with small difference
  }
  else if (user1.composite_score > limitLevel2 && user2.composite_score > limitLevel2 || diffCompScore <= diffLevel2 && user1.composite_score > limitLevel2 
    || diffCompScore <= diffLevel2 && user2.composite_score > limitLevel2){
    level = 2;
  }
  else{
    level = 1;
  }

  return level;
};

module.exports = {compCalc, individualMeasuementCalc};

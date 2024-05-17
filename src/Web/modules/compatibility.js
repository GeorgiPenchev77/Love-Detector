const fs = require("fs");
const util = require("./util.js");

const independentFactor = 25;                     // used to get the composite score of each user in the desired range
const numberOfMeasurements = 10;                  // number of measurements during the date for each user
const DATE_DURATION = 180000;                     // = 3 min. Duration of the date 

const RESULT_JSON = "./newHeartbeatData.json"     // file with current data that is to-be updated

// update values of individual measurement for according user
const individualMeasuementCalc = (userID) => {

  try {
    util.updateJSON(RESULT_JSON, (data) => {

      data.users[userID].IM_average_heartbeat = getAverageIM(data.users[userID]);
      data.users[userID].IM_heartbeat_peak = getPeakIM(data.users[userID]);
      data.users[userID].IM_upper_bracket = getUpperBracketIM(data.users[userID]);

    });
  }
  catch (error) {
    console.log(error);
    throw error;
  }
};

// update values of date measurement for both users
const compCalc = () => {

  try {
    util.updateJSON(RESULT_JSON, (data) => {

      data.users.forEach(user => {

        user.date_average_heartbeat = getAverageDate(user);
        user.date_heartbeat_peak = getPeakDate(user);
        user.date_upper_bracket = getUpperBracketDate(user);
        user.date_spike_counter = getSpikes(user);
        user.composite_score = getCompositeScore(user);

      });

      data.match_result = getDateResult(data.users);
      data.test_data_for_graph.number_of_points = numberOfMeasurements;
      data.test_data_for_graph.time_seconds = DATE_DURATION/1000;  //we divide by a 1000 to turn milliseconds into seconds
    });
  }
  catch (error) {
    console.log(error);
    throw error;
  }
};

// get the average from an array of values
const calcAvarage = (array) => {                           
  let avg = 0;
  for (let i = 0; i < array.length; i++) {
    avg += array[i];
  }
  return parseInt(avg / array.length);
}

// calculate average heart rate during Individual Measurements
const getAverageIM = (user) => {                           
  const arrayHb = user.IM_heartbeat_data;
  return calcAvarage(arrayHb);
};

// calculate average heart rate during date
const getAverageDate = (user) => {                         
  const arrayHb = user.date_heartbeat_data;
  return calcAvarage(arrayHb);
};

// function to find the highest heart rate during Individual Measurement
const getPeakIM = (user) => {                              
  let peak = 0;
  const arrayHb = user.IM_heartbeat_data;
  for (let i = 0; i < arrayHb.length; i++) {
    if (peak < arrayHb[i]) {
      peak = arrayHb[i];
    }
  }
  return peak;
};

// find the highest heart rate during date
const getPeakDate = (user) => {                           
  let peak = 0;
  const arrayHb = user.date_heartbeat_data;
  for (let i = 0; i < arrayHb.length; i++) {
    if (peak < arrayHb[i]) {
      peak = arrayHb[i];
    }
  }
  return peak;
};

// find the range between average and highest heart rate during Individual Measurements 
const getUpperBracketIM = (user) => {                    
  const indAverageHb = getAverageIM(user);
  const indPeak = getPeakIM(user);

  let upperBracket = indAverageHb + ((indPeak - indAverageHb) / 2);
  return parseInt(upperBracket);
};

// find the range between average and highest heart rate during Individual Measurements
const getUpperBracketDate = (user) => {                   
  const indAverageHb = getAverageDate(user);
  const indPeak = getPeakDate(user);

  let upperBracket = indAverageHb + ((indPeak - indAverageHb) / 2);
  return parseInt(upperBracket);
};

// function to calculate the amount of times difference between measure k and k-1 is drastic
const getSpikes = (user) => {                            
  let spikeCounter = 0;
  const arrayHb = user.date_heartbeat_data;

  for (let i = 1; i < arrayHb.length; i++) {
    const difference = Math.abs(arrayHb[i] - arrayHb[i - 1]);
    if (difference >= 15) {
      spikeCounter = spikeCounter + 1;
    }
  }
  return spikeCounter;
};

// counts how many heart rate measurements during the date are part of the upper bracket
const countUpperBracketIndex = (user) => {                
  const upperBracket = getUpperBracketDate(user);
  let aboveUpperBracket = 0;
  const arrayHb = user.date_heartbeat_data;
  for (let i = 0; i < arrayHb.length; i++) {
    if (arrayHb[i] > upperBracket) {
      aboveUpperBracket++;
    }
  }
  return aboveUpperBracket;
};

// takes all functions and calculates a score roughly between 0-15
const getCompositeScore = (user) => {                    
  const maxDiff = 60;                                    // normal heartbeat is between 60-100 bpm, im accounting for a bit more
  const TRHIndex = countUpperBracketIndex(user);

  let normalizedAvgDiff = Math.abs(getAverageDate(user) - getAverageIM(user)) / maxDiff;
  let normalizedPeakDiff = Math.abs(getPeakDate(user) - getPeakIM(user)) / maxDiff;

  let compositeScore = Math.abs(((getSpikes(user) / numberOfMeasurements) * (normalizedPeakDiff - normalizedAvgDiff)) + (TRHIndex / numberOfMeasurements)) * independentFactor;

  return compositeScore;
};

// calculates whether or not both values are above a threshhold and if they are far apart
const getDateResult = (users) => {                      
  let level = 0;                                        // this level indicates the result of the date
  const user1 = users[0];
  const user2 = users[1];
  const diffCompScore = user1 - user2;

  const limitLevel3 = 15 * 0.75;
  const limitLevel2 = 15 * 0.25;
  const diffLevel3 = 3;
  const diffLevel2 = 5;

  // the level is determined based on the composite scores of the users and the difference between them
  if (user1.composite_score > limitLevel3 && user2.composite_score > limitLevel3 || diffCompScore <= diffLevel3 && user1.composite_score > limitLevel3
    || diffCompScore <= diffLevel3 && user2.composite_score > limitLevel3) {
    level = 3;
  }
  else if (user1.composite_score > limitLevel2 && user2.composite_score > limitLevel2 || diffCompScore <= diffLevel2 && user1.composite_score > limitLevel2
    || diffCompScore <= diffLevel2 && user2.composite_score > limitLevel2) {
    level = 2;
  }
  else {
    level = 1;
  }

  return level;
};

module.exports = { compCalc, individualMeasuementCalc, DATE_DURATION };


fillUserData();

//A graph representation of how the heartbeats behaved throughout the date. 
async function drawChart(userData) {

  const numberOfPoints = userData.test_data_for_graph.number_of_points;
  const testTime = userData.test_data_for_graph.time_seconds;



  var data1 = google.visualization.arrayToDataTable([]); // initialize empty graph to later be filled with data from JSON
  data1.addColumn('number', 'Seconds');
  data1.addColumn('number', 'User1_BPM');
  data1.addColumn('number', 'User2_BPM');

  for (let i = 0; i <= numberOfPoints; i++) {
    data1.addRow();
    let time = testTime / numberOfPoints * i;
    data1.setCell(i, 0, time);
    if (i == 0) {
      data1.setCell(i, 1, userData.users[0].IM_average_heartbeat);
      data1.setCell(i, 2, userData.users[1].IM_average_heartbeat);
    } else {
      data1.setCell(i, 1, userData.users[0].date_heartbeat_data[i - 1]);
      data1.setCell(i, 2, userData.users[1].date_heartbeat_data[i - 1]);
    }
  }

  /*Designates technical aspects of graph; how the curve looks like, colors, size...*/
  var options1 = {
    curveType: 'function',
    legend: { position: 'bottom' },
    colors: ['#a92a85', '#fa71c8'],
    backgroundColor: '#FFFFFF',
    chartArea: { width: '80%', height: '70%' }, // Adjust chart area
    vAxis: { format: '#', ticks: [40, 60, 80, 100, 120, 140] }
  };

  var chart1 = new google.visualization.LineChart(document.getElementById('chartDiv1'));
  chart1.draw(data1, options1); /*Creates the line for the chart, with the properties specified before*/
}

//Retrieve the user from the heartBeatData.json 
async function getUserData() {
  try {
    const response = await fetch("/getUserData", {
      method: "GET",
      headers: {
        "Content-Type": "application/json"
      },
    });

    if (!response.ok) {
      throw new Error("Failed to retrieve user data.");
    }

    return await response.json();
  } catch (error) {
    console.error("Error loading user data:", error);
  }
}

//Display the collected heartbeat data for each user. 
async function fillUserData() {
  const userData = await getUserData();

  google.charts.load('current', { 'packages': ['corechart'] })
    .then(_ => drawChart(userData));

  let userName1 = userData.users[0].username;
  let userName2 = userData.users[1].username;
  let userPronouns1 = userData.users[0].pronouns;
  let userPronouns2 = userData.users[1].pronouns;
  let userNormalHeartbeat1 = userData.users[0].IM_average_heartbeat;
  let userNormalHeartbeat2 = userData.users[1].IM_average_heartbeat;
  let userDateHeartbeatAvg1 = userData.users[0].date_average_heartbeat;
  let userDateHeartbeatAvg2 = userData.users[1].date_average_heartbeat;
  let userHeartBeatPeak1 = userData.users[0].date_heartbeat_peak;
  let userHeartBeatPeak2 = userData.users[1].date_heartbeat_peak;

  document.getElementById("userName1").textContent = userName1;
  document.getElementById("userPronouns1").textContent = userPronouns1;
  document.getElementById("normalHeartbeat1").textContent = userNormalHeartbeat1;
  document.getElementById("dateHeartbeatAvg1").textContent = userDateHeartbeatAvg1;
  document.getElementById("heartbeatPeak1").textContent = userHeartBeatPeak1;

  document.getElementById("userName2").textContent = userName2;
  document.getElementById("userPronouns2").textContent = userPronouns2;
  document.getElementById("normalHeartbeat2").textContent = userNormalHeartbeat2;
  document.getElementById("dateHeartbeatAvg2").textContent = userDateHeartbeatAvg2;
  document.getElementById("heartbeatPeak2").textContent = userHeartBeatPeak2;

  displayMatchResult(userData);
}


//Display the match level depending on the compatibility result. 
async function displayMatchResult(userData) {

  const matchResult = userData.match_result;
  const resultContainer = document.querySelector('.result-container');
  const resultMessage = document.getElementById('resultMessage');
  const levelMessage = document.getElementById('level');
  let message = '';
  let level = '';

  switch (matchResult) {
    case 1:
      message = 'Maybe not the best match...';
      level = 'Level 1';
      break;
    case 2:
      message = 'There is potential for a nice connection here!';
      level = 'Level 2!';
      break;
    case 3:
      message = 'A high chance of compatibility detected!';
      level = 'Level 3!!';
      break;
    default:
      message = 'Unable to calculate compatibility level';
      level = 'Level 0:';
  }

  resultMessage.textContent = message;
  levelMessage.textContent = level;
}

//Redirect to the start page again. 
function goHome() {
  window.location.href = "./index.html";
}

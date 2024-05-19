const socket = io();

socket.on('start', function () {
  goToQuestions();
});

//Retrieve user info from server
async function getUserData() {
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
  }

  //Take the retrieved user data and display it on the screen
  async function fillUserData() {
    let userData = await getUserData();
    console.log("User data:", userData);

    let userName1 = userData.users[0].username;
    let userName2 = userData.users[1].username;
    let userPronouns1 = userData.users[0].pronouns;
    let userPronouns2 = userData.users[1].pronouns;

    document.getElementById("userName1").textContent = userName1;
    document.getElementById("userPronouns1").textContent = userPronouns1;
    document.getElementById("userName2").textContent = userName2;
    document.getElementById("userPronouns2").textContent = userPronouns2;

    individualMeasurementsMax = userData.individual_measurements;
  }

  fillUserData();

function goBack() {
  window.history.back();
}

function goToQuestions(){
  window.location.href = "./questions.html";
}
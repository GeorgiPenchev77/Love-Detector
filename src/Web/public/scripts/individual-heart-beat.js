const socket = io();
    socket.emit('startIM');
    let userId = "user1";
    let started = false;
    let individualMeasurementsMax;
    let progressCounter = 0;
    let previousProgressCounterLeft = 0;
    let previousProgressCounterRight = 0;

    socket.on("start", () => {
      startMeasuring();
      console.log("start");
    });

    socket.on("stop", () => {
      pauseMeasuring();
      console.log("pause");
    });

    socket.on("progress", () => {
      progressMeasuring();
      console.log("progress");
    });

    //Retrieve user data from JSON via server
    async function getUserData() {
      let response = await fetch("/getUserData", {
        method: "GET",
        headers: {
          "Content-Type": "application/json",
        },
      });

      if (!response.ok) {
        throw new Error("Failed to retrieve user data.");
      }
      return await response.json();
    }

    //Display retrieved user data on the UI
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
    document.getElementById(userId).classList.add("active");

  //Switch between user 1 and 2 to measure one individual heartbeat after another. 
  async function changeUser(){
      if(started){
        alert("Please pause the test first, before trying to switch users.");
        return;
      }
      socket.emit('changeCurrentUser');
      document.getElementById(userId).classList.remove('active');
      swapUsers();
      document.getElementById(userId).classList.add('active');
    }

    function swapUsers(){
      if(userId == "user1"){
        progressCounter = 0;
        userId = "user2";
      }
      else{
        progressCounter = 0;
        userId = "user1";
      }
      socket.emit('resetIM');
    }

    function goBack() {
      socket.emit('endIM');
      window.history.back();
    }

    /*Switch the content of each user container when they start the individual measuring on terminal
    to display the progress of the measuring*/
    function startMeasuring() {
      if (
        document.getElementById(userId).classList.contains("active") &&
        !started
      ) {
        started = true;

        let currentContainer = document.getElementById(userId);
        let instructionContainer = currentContainer.querySelector(
          ".instruction-container"
        );

        instructionContainer.innerHTML = "";

        // Create and set the progress percentage 
        let progressPercent = document.createElement("p");
        progressPercent.textContent = progressCounter * 20 + "%";
        progressPercent.classList.add("progress"); // Add a CSS class for styling

        // Create and set the message paragraph 
        let messageParagraph = document.createElement("p");
        messageParagraph.textContent =
          "Heartbeat measuring has been started. \nProgress: ";
        messageParagraph.classList.add("message-text"); // Add a CSS class for styling

        instructionContainer.appendChild(messageParagraph);
        instructionContainer.appendChild(progressPercent);
      }
    }

    //Control the visual representation in webpage when measuring has been paused.
    function pauseMeasuring(){ 
      if(document.getElementById(userId).classList.contains('active') && started){
        started = false;
        let currentContainer=document.getElementById(userId);
        let instructionContainer = currentContainer.querySelector('.instruction-container');
        instructionContainer.innerHTML = "";

        let messageParagraph = document.createElement("p");
        messageParagraph.textContent =
          "Heartbeat measuring is paused. Restart it to complete the test.";
        messageParagraph.classList.add("message-text");
        instructionContainer.appendChild(messageParagraph);
      }
    }

    //Display the final individual measurement result when the individual measuring test is complete.
    async function completeMeasuring() {
      if (
        document.getElementById(userId).classList.contains("active") &&
        started
      ) {
        let currentContainer = document.getElementById(userId);
        let instructionContainer = currentContainer.querySelector(
          ".instruction-container"
        );
        instructionContainer.innerHTML = "";

        let result = await getIndividualMeasurement();

        let messageParagraph = document.createElement("p");
        messageParagraph.textContent = "Normal heartbeat: " + result;
        messageParagraph.classList.add("message-text");
        instructionContainer.appendChild(messageParagraph);
        resetMeasuring();
      }
    }

    //Reset measuring progress bar, when it has been complete or invalid
    function resetMeasuring(){ 
      progressCounter = 0;
      started = false;
    }

    //Retrieves each users average heartbeat 
    async function getIndividualMeasurement() {
      let userData = await getUserData();
      if(userId === "user1"){
        return userData.users[0].IM_average_heartbeat;
      } else {
        return userData.users[1].IM_average_heartbeat;
      }
    }

    //Updates the progress of the Individual measurement in each user box on the UI. 
    function progressMeasuring() {
      progressCounter++;
      if (progressCounter < individualMeasurementsMax) {
        updatePercent();
      } else {
        completeMeasuring();
      }
    }

    //Show the progress in percent
    function updatePercent() {
      let percent = document.getElementsByClassName("progress")[0];
      console.log(percent);
      percent.innerHTML = progressCounter * 20 + "%";
    }

    //Indicates to the server that the individual measurement has ended upon relocation to the next page.
    function goToStartDatePage(){
      socket.emit('endIM')
      window.location.href="./start-date-page.html"
    }

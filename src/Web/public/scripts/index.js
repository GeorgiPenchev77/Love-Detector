const socket = io();
  socket.on("message", (msg) => {
  console.log(msg);
  });

  //Save the username and pronouns to json file. 
  function saveUserInfo() {
    var userData = {
      user1: {
          username: document.getElementById("userName1").value,
          pronouns: document.getElementById("userPronouns1").value
      },
      user2: {
          username: document.getElementById("userName2").value,
          pronouns: document.getElementById("userPronouns2").value
      }
    };

    if (!userData.user1.username || !userData.user2.username) {
        alert("Please fill in all name fields.");
        return;
    }

    var jsonData = JSON.stringify(userData, null, 2);

    // Send JSON data to server
    fetch("/saveUserData", {
      method: "POST",
      headers: {
          "Content-Type": "application/json"
      },
      body: jsonData
    })
    .then(response => {
      if (!response.ok) {
          throw new Error("Failed to save user data.");
      }
      return response.json();
    })
    .then(data =>{
      console.log("userdata saved successfully:", data);
      window.location.href = "./individual-heart-beat.html";
    })
    .catch(error => {
      console.error("Error:", error);
    });
  };
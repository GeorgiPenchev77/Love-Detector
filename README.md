![Love Detector Official Logo](documents/images/Love-Detector-Logo.png)

# Love Detector: The New Speed-Dating Tool
### Table of Contents
- [Table of Contents](#table-of-contents)
- [Project Description](#project-description)
- [Purpose](#purpose)
- [Benefits](#benefits)
- [Dependencies](#dependencies)
- [Used Technologies](#used-technologies)
- [Installation](#installation)
  1. [Installation: Wio Seeed Terminal](#installation-wio-seeed-terminal)
  2. [Installation: Web Application](#installation-web-application)
- [Usage](#usage)
  1. [Usage: Wio Seeed Terminal](#usage-wio-seeed-terminal)
  2. [Usage: Web Application](#usage-web-application)
  3. [Demo and Examples](#demo-and-examples)
- [Hardware and Software Diagrams](#hardware-and-software-diagrams)  
- [Full Documentation](#full-documentation)
- [License](#license)
- [Team Members and Contributions](#team-members-and-contributions)

## Project Description:
***The Love Detector*** is an all-in-one system that adds a fun twist to speed dating. It measures your heartbeat as you engage in conversations guided by questions displayed on the web app screen, suggesting whether there might be a spark with your potential match. Before the **"date"** starts, sensors are attached to the users' earlobes, and they are provided with a terminal and a computer with the web app open. Following instructions on both the terminal and webpage, users' heartbeats are initially measured individually to establish a baseline.

Firstly, the user will follow the instructions on both the ***Wio Seeed Terminal*** and ***Web-Based User Interface***. Then, we measure the heartbeats of both users individually to establish a baseline for comparison, with the sensors attached to the users' earlobes. During the date, users’ heartbeats are continuously monitored while they read and discuss our thought-provoking questions. All of this new data is collected and saved in arrays inside JSON files, our server then takes the data from this file and publishes it. This act makes it possible to send the data through our MQTT broker, where our terminal is subscribed to the relevant topics. This connection between: Server, broker, terminal, allows us to send information seamlessly and effectively. 

After careful analysis using a pre-established [***Love Detection Algorithm***](https://git.chalmers.se/courses/dit113/2024/group-4/love-detector/-/wikis/Love-Detector-Algorithm), the results are calculated based on variations and spikes of heartbeat between each user, which provides us with a composite score. Both users composite scores are then compared based on how close they are to each other, resulting in a match rating from 1 to 3, indicating the potential strength of the connection: no match, decent match, or absolute match.

The results are displayed on the UI, which has its own dedicated page where the user will be able to see a heartbeat graph and some general information about the date. On the terminal side of things LEDs will indicate the match level reached by the users. By the end of the date, user will gain a better understanding of how their connection is perceived by their hearts, helping them decide their next steps.

## Purpose:
The purpose of our project, ***The Love Detector*** is to create a scientifically driven, analytical tool that assesses the likelyhood of a deeper connection or a so-called **"spark"** between two individuals during a speed-date. By combining continious *heartbeat detection*, a rotating *list of questions*, and a special *algorithm*, the project relies on the *physical reactions* our body exerts in moments of excitement, 
to achieve the best possible *deduction*. 

## Benefits:
***The Love Detector*** offers several significant benefits that enhance the speed dating experience:

  1. **Scientific Insight**: By utilizing scientifically driven analysis of physiological responses such as **BPM**, the Love Detector provides an objective measure of physical reactions participants have during interaction. 
  
  2. **Enhanced Conversations** By presenting thought-provoking questions, ***The Love Detector*** facilitates meaningful dialog where participants can discover common interests and thus form deeper connections.
  
  3. **Novelty and Fun** Including a technological aspect to the traditional speed dating format adds a new exciting twist to the concept, making the experience more enjoyable and memorable. 
  
  4. **Authentic Connections** By providing real-time feedback on physiological reactions, ***The Love Detector*** helps to capture **meaningful and authentic connections** that might otherwise go unnoticed in the fast-paced setting of speed-dates and our lives in general. 

## Dependencies: 
The following section lists the dependencies and requirements for the project. These should be installed prior to the 
installation process described in the next section:
 1. [Wio Seeed Terminal](https://www.seeedstudio.com/Wio-Terminal-p-4509.html) (or a compatible alternative board)

 2. [Arduino IDE](https://www.arduino.cc/en/software) or [Arduino CLI](https://github.com/arduino/arduino-cli)

 3. **Wio Terminal Board Library**: follow the installation tutorial [here](https://wiki.seeedstudio.com/Wio-Terminal-Getting-Started/#getting-started)

 4. All of the following libraries need to be installed in the **Arduino IDE**:
   - *TFT_eSPI* - [link](https://github.com/Bodmer/TFT_eSPI)
   - *rpcWifi* - [link](https://github.com/Seeed-Studio/Seeed_Arduino_rpcWiFi)
   - *Arduino MQTT Client* - [link](https://github.com/arduino-libraries/ArduinoMqttClient)
   - *Adafruit_NeoPixel* - [link](https://github.com/adafruit/Adafruit_NeoPixel)

 5. The needed Grove Sensors for the project can be found [here](#system-sensors)

 6. [Git](https://git-scm.com/downloads) should be installed and working on your machine so the repository of the project can be cloned.

 7. [Node.js](https://nodejs.org/en) and [npm](https://www.npmjs.com/) should be installed.
   - *Note:* **Node.js** comes standardly with **npm** as a package, so you most likely don't need to download it separately.
   - *Note:* We strongly recommend that you go with the **latest stable version** of **Node.js** 

## Used Technologies:
The following list contains the technologies used in this project:
  - C++
  - JavaScript
  - HTML/CSS
  - Figma
  - Wio seeed terminal
  - Grove LED Stick - [RGB LED Stick (10 – WS2813 Mini)](https://wiki.seeedstudio.com/Grove-RGB_LED_Stick-10-WS2813_Mini/) 
  - Grove Heart Rate Sensor - [Ear-clip Heart Rate Sensor](https://wiki.seeedstudio.com/Grove-Ear-clip_Heart_Rate_Sensor/) Note: two(2) of these sensors are needed.
  
## Installation:
The following section lists the needed steps in order to complete the installation process for the project:

  ### *Installation: Wio Seeed Terminal*
  Ensure that all libraries are installed by following these steps:
  1. Open *Arduino IDE*, go to *Tools -> Manage Libraries* .
  2. From the list of [*Dependecies*](#dependencies), under *Section 4*, search the name of the library and click *Install*.
  3. Repeat this for all libraries listed.(It is best to install the latest versions of the libraries).

---

  ### *Installation: Web Application*
  1. Ensure you have *Node.js* and *npm* installed. If not, download and install them from [here](https://nodejs.org/en/).
  2. Navigate to the directory of the project in your *terminal*.
  3. Run *npm install* in your *terminal* to install all the required dependencies listed in the [package.json](src/Web/package.json) file.

## Usage:
The following section presents a short guide on how to use our project: 
***Note:*** Make sure you have completed the [*Installation*](#installation) guides to be able to use the project.

  ### *Usage: Wio Seeed Terminal*
  1. Connect the *Wio Seeed Terminal* using a USB-C cable to your machine.
  2. Open *Arduino IDE* and navigate to *File -> Open*.
  3. Go to the project folder and open *Wio* Folder (with path: love-detector/src/Wio).
  4. Navigate to *Select Board* and make sure the *Wio Seeed Terminal* is selected.
  5. Locate the *love_detector.ino* file and run it by using the *Upload* button.

---

  ### *Usage: Web Application*
  1. In your *Terminal* navigate to the *Web* folder (with path: love-detector/src/Web).
  2. Run *npm start* in the terminal.
  3. Open your browser and enter the address *localhost:[PORT]*. Replace *[PORT]* with the port number in the terminal.
  4. To stop the application, press *CTRL + C* in the terminal.    

---

  ## *Demo and Examples*
  In order to create a full demonstration of how the project works, the team has created a video.
  This video is based on an example [scenario](https://git.chalmers.se/courses/dit113/2024/group-4/love-detector/-/wikis/home). To view the video, click [here](https://www.youtube.com/watch?v=uyDf9rb2Hco).

## Hardware and Software Diagrams:
The following section contains the design diagram, that the team followed during the lifecycle of the project:

![Design Diagram Final Version](documents/images/Design-Diagram.png)

## Full Documentation:
For the full documentation of the project, see our [Wiki](https://git.chalmers.se/courses/dit113/2024/group-4/love-detector/-/wikis/home).
You can navigate between the different pages, using the panel on the right side.

## License:
The project is licensed under the MIT License. Refer to the [**LICENSE**](documents/License) file for more information.

## Team Members and Contributions:
- **Georgi Penchev** (penchev@student.chalmers.se): Main contributions made in the Back-End with the Wio terminal, including taking the lead on computing the heartbeat sensors as well as assisting team members with other parts of hardware-related programming such as LED lights and communication from Wio terminal to broker. Additionally, contributed to the video production, taking on the role of an actor.
- **Kristina Trampush** (gustramkr@student.gu.se): Main contributions made in the Front-End, including taking the lead on development of the Web application, including communication between server and UI as well as UI design.
- **Pavlo Ovchynnykov** (gusovchpa@student.gu.se): Main contributions made in MQTT connectivity as well as the Back-End with the Wio terminal, including continuously handling new connections between the different components of the system as well as computing the heartbeat sensors. Additionally, contributed to the video production, taking on the role of an actor. 
- **René Ignacio González Pérez** (renei@student.chalmers.se): Main contributions made in the Web application with a special focus on computing the compatibility algorithm, as well as contributing to the UI design and computing of LED lights in the Wio terminal. Additionally, contributed to in video making, taking on the role of director, editor and actor.
- **Telina Hjärtberg** (telina.hjartberg@icloud.com): Main contributions made in the Web application with a focus on UI design, as well as computing the LED lights in the Wio terminal. Additionally,contributed to in video making, directing the video and filming. 

*Love Detector 2024*, **DIT113, University of Gothenburg | Chalmers University of Technology, Sweden**  
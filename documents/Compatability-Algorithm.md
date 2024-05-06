### Love Detector Algorithm

Introduction: The Love Detector Algorithm is a technology-driven analytical tool created to assess the likelihood of a spark between two independent users throughout the duration of a speed date. This document outlines the methodology, questionnaire, scoring system, and interpretation criteria used in the algorithm.

### Methodology:

- Questionnaire Development: A comprehensive questionnaire is developed to capture different topics and areas of interest for the two users that will initiate interesting and genuine conversations.
- Heartbeat Measurement: The Love Detector leverages advanced heartbeat measurement technology to monitor the physiological responses of participants during their brief interactions. Heart rate variability (HRV) and pulse rate are key metrics analyzed to gauge excitement levels.
- Sensor Integration: High-quality heartbeat sensors are strategically integrated into our speed-dating tool, discreetly monitoring participants' heart rates throughout the duration of each date. Real-time data acquisition ensures accuracy and responsiveness.
- Data Collection: The two individuals independently answer calibrating questions. Their physiological responses are saved to enhance the test and guarantee a more accurate result.
- Data Processing: Collected heartbeat data is processed using a robust and interpretive algorithm to extract meaningful insights regarding participants' excitement levels and potential romantic interest. Statistical analysis and pattern recognition techniques are employed for robust assessment.

### **Algorithm construction:**

\~ IM(Individual Measurements) - statistic for the individual testing of the person that contains the following information:

- I-HRV (Individual Heart Rate Variability) - how often does the user's heart \*spike when in a controlled environment.
- I-APR (Individual Average Pulse Rate) - The average of the user's heart rate measurements over the length of the test in the controlled environment.
- I-HPR (Individual Highest Pulse Rate) - The highest measurement of the user's heart rate over the length of the test in the controlled environment.

\~ TM(Test Measurements) - statistics that will be analyzed during the date and will serve to generate an appropriate result. Based on but not limited to:

- TRH-index - The index will serve the purpose of storing how often the user's heart rate was in the \*upper bracket of their IM.
- T-HPR (Test Average Pulse Rate) - The average of the user's heart-rate measurements over the length of the test in a speed-date environment.
- T-APR (Test Highest Pulse Rate) - The highest measurement of the user's heart rate over the length of the test in the test environment.
- HRV (Heart Rate Variability) - the I-HRV, however, measured in a speed-date environment.
- \*TD (Test Duration) - the amount of time that the speed-date took,

\--

\*spike: rise or drop \>15 heart-rate "points" in between measurement k and k+1, where k\>0.

\*upper bracket of their IM: I-APR + ((I-HPR - I-APR)/2) = upper bracket of the users controlled pulse rate.

\*TD: test duration is 5 minutes or 300 seconds.

### **Algorithm:**

Composite Score = w0\*((HRV/I-HRV)\*\_((T-HPR - I-HPR) + (T-APR - I-HPR)))+ w1\*\_TRH-index,

where w0 and w1 is an independent constant coefficient that indicates the importance of each factor.

### **Interpretation Criteria:**

Composite Score > Threshold: Indicates a high likelihood of a spark and mutual excitement in both participants. 

Composite Score ≤ Threshold: Suggests that certain essential elements associated with love have not been identified during the test.

### IMPORTANT NOTES:

**_Limitations:_** The algorithm provides insights and suggestions based on quantitative data but cannot capture the entirety of human emotions and relationships. Contextual factors such as cultural differences, individual personalities, external circumstances many more should be considered when interpreting the results.

---

**_Purpose:_**The purpose of the Love Detector is to draw smiles on their users' faces, bring them closer, and be an exciting and fun way to meet new people, make new friends, and create meaningful relationships - no matter what kind. In case you don't agree with the result our tool has calculated, please select the appropriate option at the end of your date and understand that technology cannot be perfect, but you are :).

---

**_Feedback and Iteration:_** Feedback from users who have used the algorithm together with our speed-dating tool and have agreed to share their results will be stored and used to improve the algorithm, questionnaire, and overall quality of the system.
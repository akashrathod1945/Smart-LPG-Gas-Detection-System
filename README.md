oT-Based LPG Gas Leakage Detection and Safety Automation                                         System Using ESP32 
 

Akash Rathod 
Department of Electronics and Communication Engineering 
Mahatma Gandhi Institute of Technology, Hyderabad, India 

Abstract 
Liquefied Petroleum Gas (LPG) is widely used in residential, commercial, and industrial applications. However, gas leakage can lead to fire hazards, explosions, and health risks. This paper presents an IoT-based LPG Gas Leakage Detection and Safety Automation System using the ESP32 microcontroller and MQ gas sensor. The proposed system continuously monitors LPG concentration levels and categorizes the environment into multiple safety levels. Upon detection of hazardous gas concentrations, the system automatically activates a ventilation fan, shuts off the gas valve using a servo motor, and sends emergency notifications to users through mobile applications and SMS alerts. The system also uploads real-time sensor data to the ThingSpeak cloud platform for remote monitoring and analysis. Experimental results demonstrate the effectiveness of the system in providing rapid response and enhanced safety. 

Keywords: LPG Leakage Detection, ESP32, MQ Sensor, IoT, ThingSpeak, Safety Automation, Servo Motor. 

I. Introduction 
Liquefied Petroleum Gas (LPG) is one of the most commonly used fuels for cooking and industrial applications due to its high calorific value and clean-burning characteristics. Despite its advantages, LPG leakage remains a major safety concern. Gas leaks may occur due to damaged pipelines, faulty regulators, or improper handling of cylinders. 

Traditional gas leakage detection systems mainly provide local alarms without implementing automatic preventive actions. Recent advancements in the Internet of Things (IoT) have enabled intelligent monitoring systems capable of real-time data acquisition, cloud connectivity, and automated emergency responses. 

This paper proposes an IoT-based LPG Gas Leakage Detection and Safety Automation System that not only detects gas leakage but also performs automatic safety actions such as shutting off the gas supply, activating ventilation mechanisms, and notifying users remotely. 

II. Literature Survey 
Several LPG leakage detection systems have been proposed in previous studies. Most systems utilize gas sensors and microcontrollers to detect leakage and trigger alarms. However, many existing solutions lack cloud monitoring capabilities and automatic safety mechanisms. 

Recent IoT-based systems have improved monitoring capabilities through cloud integration. However, limitations such as delayed response, absence of automatic gas shut off, and limited risk classification still exist. The proposed system addresses these shortcomings by integrating real-time cloud monitoring, multi-level risk assessment, automatic gas valve control, and emergency notification features. 

III. Proposed System 
The proposed system consists of: 

ESP32 Microcontroller 
MQ Gas Sensor 
Servo Motor 
Exhaust Fan 
Buzzer 
ThingSpeak Cloud Platform 
GSM/Mobile Notification Module 
Working Principle 
The MQ sensor continuously monitors LPG concentration. 
The ESP32 reads sensor values and compares them with predefined thresholds. 
Based on gas concentration levels, the system classifies the environment into: 
Safe Level 
Warning Level 
Danger Level 
Critical Level 
During warning conditions, visual and audible alerts are generated. 
During danger conditions: 
Ventilation fan is activated. 
Emergency notifications are sent. 
During critical conditions: 
The server motor closes the gas valve. 
Fan remains activated. 
Continuous emergency alerts are issued. 
Sensor readings are uploaded to ThingSpeak for real-time monitoring. 
IV. Hardware Components 
A. ESP32 Microcontroller 
ESP32 serves as the central controller of the system. It processes sensor data, controls actuators, and enables Wi-Fi communication with cloud platforms. 

B. MQ Gas Sensor 
The MQ sensor detects LPG concentration in the surrounding environment and converts gas concentration into analog voltage signals. 

C. Servo Motor 
The servo motor is used for automatic gas valve shut-off during critical leakage conditions. 

D. Ventilation Fan 
The fan helps remove accumulated gas from the environment, reducing explosion risks. 

E. Buzzer 
The buzzer provides immediate local alerts whenever leakage is detected. 

V. Circuit Diagram 

 

VI. System Architecture 
The architecture of the proposed system includes gas sensing, signal processing, cloud communication, and safety automation modules. 

Gas Leakage → MQ Sensor → ESP32 Controller → Decision Logic 

→ Buzzer Alert 

→ Ventilation Fan Control 

→ Servo Motor Gas Shut-Off 

→ ThingSpeak Cloud Monitoring 

→ SMS/Mobile Notifications 

 

VII. Software Implementation 
The system software is developed using Arduino IDE. 

Algorithm 
Initialize ESP32 and sensor modules. 
Read MQ sensor value. 
Compare sensor value with threshold levels. 
If gas concentration exceeds warning level: 
Activate buzzer. 
If gas concentration exceeds danger level: 
Start ventilation fan. 
Send notification. 
If gas concentration exceeds critical level: 
Close gas valve using servo motor. 
Upload sensor data to ThingSpeak. 
Repeat continuously. 
 
 
VIII. Experimental Results 
The system was tested under various LPG concentration levels. 

Gas Level 
Sensor Reading 
System Action 
Safe 
Low 
Monitoring Only 
Warning 
Medium 
Buzzer Activated 
Danger 
High 
Fan Activated + Notification 
Critical 
Very High 
Gas Valve Closed + Emergency Alert 
The results indicate that the proposed system successfully detects LPG leakage and performs safety actions with minimal response time. Real-time cloud monitoring through ThingSpeak enables users to remotely observe gas concentration trends. 

 

 
IX. Advantages 
Real-time gas leakage detection. 
Automatic gas valve shut-off. 
Cloud-based monitoring. 
Mobile notifications and alerts. 
Reduced risk of fire and explosions. 
Low-cost implementation. 
Easy installation and maintenance. 
X. Applications 
Residential kitchens. 
Restaurants and hotels. 
Industrial gas storage facilities. 
Laboratories. 
Commercial buildings. 
XI. Future Scope 
Future enhancements may include: 

AI-based leakage prediction. 
Integration with smart home systems. 
Battery backup during power failures. 
Mobile application with live analytics. 
Multi-gas detection capability. 
XII. Conclusion 
This paper presented an IoT-based LPG Gas Leakage Detection and Safety Automation System using ESP32 and MQ gas sensors. The system effectively detects gas leakage, performs automatic safety actions, and provides real-time cloud monitoring. The integration of servo-controlled gas shut-off, ventilation control, and emergency notifications significantly improves safety and response time. Experimental results demonstrate that the proposed system is reliable, cost-effective, and suitable for residential and industrial applications. 

References 
[1] A. Kumar and S. Singh, “IoT Based Gas Leakage Detection System,” International Journal of Engineering Research, vol. 8, no. 4, pp. 45–50, 2023. 

[2] ESP32 Technical Reference Manual, Espressif Systems, 2024. 

[3] MQ Series Gas Sensor Datasheet, Hanwei Electronics. 

[4] ThingSpeak IoT Analytics Platform Documentation. 

[5] M. Sharma et al., “Smart LPG Monitoring and Leakage Detection System Using IoT,” IEEE Conference Proceedings, 2024. 

 

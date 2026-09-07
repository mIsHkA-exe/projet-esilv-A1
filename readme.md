SmartParking-project-A1-Esilv
Smart parking project merging web development and electronics sensors

DAVINCI CUSTOM GARAGE — Smart Parking
A connected smart parking system combining web development, IoT, embedded systems, electronics, and real-time data synchronization.

Project Overview
DAVINCI CUSTOM GARAGE is a connected smart parking prototype developed as part of the Cybersecurity Bachelor's program at ESILV during the 2025–2026 academic year.

The project combines a web-based parking reservation platform with an automated physical model representing a connected parking infrastructure.

The main objective is to create a system capable of connecting a digital parking management platform with a physical embedded system in order to simulate automated parking management.

The platform allows users to:

View available parking spaces.
Reserve a parking space online.
Receive a unique QR code.
Use the QR code as an access credential.
Automatically trigger the parking entrance barrier after validation.
View parking availability through the web interface.
The physical prototype detects parking space occupancy and communicates the information to the web application, allowing the digital and physical states of the parking system to remain synchronized.

Project Objectives
The project was designed to address several challenges associated with urban parking management:

Reduce the time required to find an available parking space.
Improve the management of available parking areas.
Automate access to the parking facility.
Provide users with updated parking information.
Synchronize a physical infrastructure with a web application.
Experiment with embedded systems and web technologies.
Develop a connected system combining electronics, software, databases, and communication protocols.
Respect a maximum hardware budget of €150.
The original project documentation identifies automatic vehicle detection, parking availability information, access control, continuous operation, low energy consumption, and maintainability as major service functions.

System Architecture
The system is organized around three main layers:

+--------------------------------------------------+
|                   USER LAYER                     |
|                                                  |
|        Web Interface / Reservation System        |
+-------------------------+------------------------+
                          |
                          v
+--------------------------------------------------+
|                APPLICATION LAYER                 |
|                                                  |
|       Symfony / REST API / Doctrine / MySQL      |
|                                                  |
|  - Users                                         |
|  - Reservations                                  |
|  - QR Codes                                      |
|  - Parking Spaces                                |
|  - Access History                                |
+-------------------------+------------------------+
                          |
                     JSON / API
                          |
                          v
+--------------------------------------------------+
|                 EMBEDDED LAYER                  |
|                                                  |
|        Arduino UNO R4 WiFi + ESP32-CAM          |
|                                                  |
|  - Ultrasonic Sensors                            |
|  - LEDs                                          |
|  - LCD Display                                   |
|  - Servo Motors                                  |
+-------------------------+------------------------+
                          |
                          v
                 PHYSICAL PARKING
The architecture enables communication between the web platform and the physical prototype, allowing parking-space information to be transmitted and updated across the system.

System Workflow
The general workflow of the system is:

1. User accesses the web application
                |
                v
2. User checks available parking spaces
                |
                v
3. User makes a reservation
                |
                v
4. System generates a unique QR code
                |
                v
5. User presents the QR code at the entrance
                |
                v
6. ESP32-CAM reads the QR code
                |
                v
7. Symfony API validates the reservation
                |
          +-----+-----+
          |           |
        VALID       INVALID
          |           |
          v           v
     Open barrier   Access denied
          |
          v
8. Vehicle enters the parking
          |
          v
9. Sensor detects vehicle presence
          |
          v
10. Parking-space status is updated
          |
          v
11. Data is transmitted to the server
          |
          v
12. Web interface is updated
The objective is to maintain consistency between the physical state of the parking and the information displayed on the web application.

Web Application
The web application was developed using the Symfony PHP framework and follows an MVC architecture.

Main Features
Parking-space availability display.
Online parking reservation.
Automatic QR-code generation.
User account management.
Administration interface.
Parking-space status management.
Communication with the embedded infrastructure.
Real-time synchronization between the physical model and the web platform.
The application represents the main digital interface between users and the smart parking system.

Database
The project uses a relational database managed with MySQL and Doctrine ORM.

The database centralizes the information required for the operation of the system.

Main entities
User
 |
 +---- Reservation
 |         |
 |         +---- QR Code
 |
 +---- Access History

Parking Space
 |
 +---- Availability Status
The main data categories include:

Users.
Reservations.
Parking spaces.
QR codes.
Parking-space status.
Access history.
For the academic prototype, fictitious data is used for demonstrations. No real banking information is intended to be stored.

REST API
A REST API developed with Symfony provides communication between the web platform and the physical parking prototype.

The API uses JSON for data exchange.

Main API responsibilities
Validate QR codes.
Retrieve parking-space status.
Update parking-space occupancy.
Receive events generated by the Arduino system.
Synchronize physical events with the database.
Provide updated information to the web application.
The API acts as the communication bridge between the embedded system and the application layer.

Embedded System
      |
      | HTTP / JSON
      v
Symfony REST API
      |
      +---- QR Code Validation
      |
      +---- Parking Status
      |
      +---- Occupancy Updates
      |
      +---- Embedded Events
      |
      v
Database
      |
      v
Web Application
Embedded System
The physical prototype is built around several electronic components.

Component	Purpose
Arduino UNO R4 WiFi	Main embedded controller
ESP32-CAM	QR-code reading and recognition
HC-SR04 ultrasonic sensors	Vehicle presence detection
Infrared sensors	Entrance and exit detection
Servo motors	Automated parking barriers
LEDs	Parking-space status indication
LCD 16x2	Parking information display
RGB LED strip	Parking model lighting
Breadboard	Component integration
Jumper wires	Electrical connections
The embedded software is developed using Arduino IDE and C++.

QR-Code Access Control
QR-code validation is one of the main access-control mechanisms implemented in the project.

Web Reservation
       |
       v
Unique QR Code
       |
       v
ESP32-CAM
       |
       v
QR Code Recognition
       |
       v
Symfony REST API
       |
       v
Reservation Validation
       |
       +--------------------+
       |                    |
     VALID                INVALID
       |                    |
       v                    v
Open Barrier          Reject Access
       |
       v
Record Access
When a valid QR code is detected, the system authorizes access and triggers the opening of the parking barrier.

Invalid QR codes are rejected.

The access event can then be recorded in the database.

Parking-Space Detection
Each parking space is equipped with a sensor designed to detect whether a vehicle is present.

When a vehicle is detected:

The physical state of the parking space changes.
The corresponding LED indicates the state of the space.
The embedded system processes the information.
The information is transmitted through the communication layer.
The database is updated.
The web interface can reflect the new parking status.
The tests conducted during the project demonstrated that the detection mechanism was sufficiently reliable for the scale of the prototype.

3D Modeling
Before the physical assembly of the prototype, the parking structure was modeled using SolidWorks.

The 3D modeling phase was used to design and organize:

Parking spaces.
Traffic lanes.
Entrance and exit barriers.
Sensor locations.
Electronic component supports.
Cable-management structures.
Protective covers for electronic connections.
The covers were designed to improve the appearance of the prototype while protecting and organizing the wiring.

Simulation and Testing
Several tools were used to validate the system before and during the physical implementation.

Simulation
Wokwi
Tinkercad
These platforms were used to simulate electronic components and validate circuit behavior before working with the physical hardware.

Physical Testing
Tests were conducted on:

Ultrasonic sensors.
Infrared sensors.
LEDs.
Servo motors.
LCD display.
ESP32-CAM.
Arduino UNO R4 WiFi.
Functional Tests
Test	Result
Vehicle detection	Successful
Parking-space status update	Successful
Valid QR-code recognition	Successful
Invalid QR-code rejection	Successful
Automatic barrier opening	Successful
Automatic barrier closing	Successful
Database synchronization	Successful
Web interface synchronization	Successful
Web ↔ Embedded communication	Functional
The tests demonstrated that the main components of the prototype could communicate and operate together according to the project's objectives.

Problems Encountered During Development
The development of DAVINCI CUSTOM GARAGE involved several technical challenges.

One of the main difficulties was implementing reliable real-time parking-space display and synchronization between the physical prototype and the web application.

Symfony ↔ Arduino Synchronization
The communication between the Symfony application, the REST API, and the Arduino embedded system required several development and testing phases.

Particular difficulties were encountered in:

Transmitting information between the web application and the embedded system.
Synchronizing the actual state of parking spaces with the information displayed on the website.
Updating parking availability in real time.
Maintaining consistency between the database, Symfony, and Arduino.
Handling events generated by the physical sensors.
Managing communication between several hardware and software components.
Real-Time Parking Display
The real-time display of parking-space availability was another significant development challenge.

The expected communication chain was:

Parking Sensors
       |
       v
Arduino
       |
       v
Communication Layer
       |
       v
Symfony REST API
       |
       v
Database
       |
       v
Web Interface
Maintaining synchronization throughout this entire chain proved to be one of the more challenging aspects of the project.

Although the prototype demonstrates the overall concept, this part of the system remains an important area for future improvement.

Project Status
DAVINCI CUSTOM GARAGE is an evolving academic project.

The current version demonstrates the main concept of a connected smart parking system, but several technical aspects can still be improved.

Future updates are expected to focus on:

Improving Symfony-to-Arduino communication.
Optimizing the REST API.
Improving real-time data synchronization.
Reducing synchronization delays.
Improving code structure and maintainability.
Improving database management.
Strengthening system reliability.
Improving the UI/UX.
Adding new features to the platform.
Improving the overall user experience.
The project should therefore be considered an active prototype rather than a final production-ready solution.

Contributions and Suggestions
We are open to suggestions and ideas that could help improve the project.

We particularly welcome proposals related to:

Code optimization.
Software architecture.
API design.
Symfony development.
Arduino communication.
Real-time data management.
Database optimization.
Cybersecurity.
UI/UX improvements.
User experience.
Embedded-system optimization.
New features and functionalities.
If you have an idea that could improve the project, feel free to open an Issue or submit a Pull Request.

Any constructive contribution aimed at improving the code, reliability, architecture, security, or user experience is welcome.

Budget
The project was developed under a strict hardware budget constraint of €150.

The final documented hardware budget was:

Total: €142.74 including VAT

The components included:

Arduino board.
ESP32-CAM.
Ultrasonic sensors.
Infrared sensors.
Servo motor.
LEDs.
LCD display.
Breadboard.
Jumper wires.
RGB LED strip.
The project therefore remained within the defined budget constraint.

Security and Privacy Considerations
Because this project was developed as part of a Cybersecurity Bachelor's program, security and data protection were considered during the design process.

The project documentation identifies several security and privacy requirements:

Use of fictitious data for demonstrations.
No real banking information.
Consideration of GDPR requirements.
Consideration of CNIL recommendations.
Server-side validation of QR-code access.
Controlled access to the physical parking.
Centralized management of reservations and access information.
The project documentation also identifies electrical safety and applicable infrastructure regulations as important constraints for a real-world implementation.

Current Limitations
Several limitations remain in the current prototype:

Sensor accuracy can be affected by vehicle positioning and environmental conditions.
The prototype operates at a small physical scale.
QR-code recognition depends on lighting conditions and camera distance.
The architecture has not yet been tested with a large number of simultaneous users.
Real-world network availability requirements have not been fully addressed.
The current real-time synchronization mechanism still requires optimization.
The communication between the embedded system and the web application can be further improved.
The current UI can be enhanced to provide a better user experience.
These limitations provide several opportunities for future development.

Future Improvements
Several development directions have been identified for future versions.

Artificial Intelligence
AI-based algorithms could be integrated to predict parking occupancy and optimize the allocation of available spaces.

Automatic License Plate Recognition
License-plate recognition could complement or eventually replace QR-code-based access control.

Mobile Application
A dedicated mobile application could provide users with easier access to reservations and real-time parking availability.

Cloud Infrastructure
Cloud-based services could improve scalability, availability, monitoring, and remote management.

Automated Payments
A secure automated payment system could bring the prototype closer to a real-world commercial parking solution.

Advanced Monitoring
Additional monitoring and analytics could be implemented to provide administrators with detailed information about parking usage and system performance.

Recommended Repository Structure
davinci-custom-garage/
|
├── README.md
|
├── web/
|   ├── src/
|   ├── templates/
|   ├── config/
|   └── ...
|
├── api/
|   └── ...
|
├── embedded/
|   ├── arduino/
|   └── esp32-cam/
|
├── database/
|   └── ...
|
├── hardware/
|   ├── schematics/
|   └── components/
|
├── 3D/
|   └── solidworks/
|
├── simulation/
|   ├── wokwi/
|   └── tinkercad/
|
├── documentation/
|   ├── cahier-des-charges/
|   └── rapport/
|
└── assets/
    ├── images/
    └── diagrams/
Technologies
Software
PHP
Symfony
Doctrine ORM
MySQL
SQL
REST API
JSON
C++
Arduino IDE
Python
Hardware
Arduino UNO R4 WiFi
ESP32-CAM
HC-SR04 ultrasonic sensors
Infrared sensors
Servo motors
LEDs
LCD 16x2
RGB LED strip
Breadboard
Jumper wires
Design and Simulation
SolidWorks
Wokwi
Tinkercad
Skills Demonstrated
This project provided practical experience in several technical areas:

Web application development.
PHP and Symfony development.
REST API development.
Relational database design.
SQL and Doctrine ORM.
Embedded programming.
C++ programming.
Arduino development.
ESP32 integration.
Sensor integration.
Electronic circuit design.
Real-time communication.
QR-code-based access control.
System integration.
3D modeling.
Hardware simulation.
Technical testing.
Debugging and troubleshooting.
Team collaboration.
Project management.
Team
Team 108 — ESILV

Kyllian Siewe Tiague
Vikash Srikanthan
Abdulrahman El Najjar
Adam Tiss
Academic project developed as part of the Cybersecurity Bachelor's program at ESILV, 2025–2026.

Documentation
The project documentation includes:

Project requirements specification.
Functional analysis.
Technical analysis.
Structural analysis.
Hardware component documentation.
Purchase order and budget documentation.
Web application documentation.
Embedded-system documentation.
Database documentation.
Testing and validation results.
Project development report.
Conclusion
DAVINCI CUSTOM GARAGE is an academic prototype demonstrating how web technologies, embedded systems, electronics, databases, and connected infrastructure can be combined to create a smart parking solution.

The project implements several key concepts, including:

Online parking reservation.
QR-code-based access control.
Automated barriers.
Vehicle detection.
Parking-space monitoring.
REST API communication.
Database synchronization.
Web-to-hardware communication.
Real-time parking information.
The project also provided practical experience in identifying and solving integration problems between software and hardware components.

Although several aspects of the system still require optimization, particularly the real-time display and synchronization between Symfony and the Arduino-based infrastructure, the project provides a functional foundation for future development.

The team intends to continue improving the code, communication architecture, system reliability, and UI/UX.

DAVINCI CUSTOM GARAGE is therefore not considered a finished product, but an evolving prototype open to optimization, experimentation, and future contributions.

Academic Project
DAVINCI CUSTOM GARAGE — Smart Parking

ESILV — Cybersecurity Bachelor's Program

Academic Year 2025–2026

Team 108
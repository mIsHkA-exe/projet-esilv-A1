# DAVINCI CUSTOM GARAGE — Smart Parking

> A connected smart parking system combining **web development, IoT, embedded systems, electronics, and real-time data synchronization**.

[![PHP](https://img.shields.io/badge/PHP-8.x-777BB4?logo=php\&logoColor=white)](https://www.php.net/)
[![Symfony](https://img.shields.io/badge/Symfony-Framework-black?logo=symfony)](https://symfony.com/)
[![MySQL](https://img.shields.io/badge/MySQL-Database-4479A1?logo=mysql\&logoColor=white)](https://www.mysql.com/)
[![Arduino](https://img.shields.io/badge/Arduino-UNO%20R4%20WiFi-00979D?logo=arduino\&logoColor=white)](https://www.arduino.cc/)
[![C++](https://img.shields.io/badge/C%2B%2B-Embedded-00599C?logo=cplusplus\&logoColor=white)](https://isocpp.org/)

---

## Table of Contents

* [Project Overview](#project-overview)
* [Objectives](#objectives)
* [System Architecture](#system-architecture)
* [System Workflow](#system-workflow)
* [Web Application](#web-application)
* [Database](#database)
* [REST API](#rest-api)
* [Embedded System](#embedded-system)
* [QR-Code Access Control](#qr-code-access-control)
* [Parking-Space Detection](#parking-space-detection)
* [3D Modeling](#3d-modeling)
* [Simulation and Testing](#simulation-and-testing)
* [Development Challenges](#development-challenges)
* [Project Status](#project-status)
* [Contributions and Suggestions](#contributions-and-suggestions)
* [Budget](#budget)
* [Security and Privacy](#security-and-privacy)
* [Current Limitations](#current-limitations)
* [Future Improvements](#future-improvements)
* [Repository Structure](#repository-structure)
* [Technologies](#technologies)
* [Skills Demonstrated](#skills-demonstrated)
* [Team](#team)
* [Documentation](#documentation)
* [Conclusion](#conclusion)

---

## Project Overview

**DAVINCI CUSTOM GARAGE** is a connected smart parking prototype developed as part of the **Cybersecurity Bachelor's program at ESILV** during the 2025–2026 academic year.

The project combines a **web-based parking reservation platform** with an **automated physical model** representing a connected parking infrastructure.

The main objective is to connect a digital parking management platform with a physical embedded system in order to simulate automated parking management.

### Main Features

The platform allows users to:

* View available parking spaces.
* Reserve a parking space online.
* Receive a unique QR code.
* Use the QR code as an access credential.
* Automatically trigger the parking entrance barrier after validation.
* View parking availability through the web interface.
* Synchronize physical parking-space occupancy with the web application.

The physical prototype detects parking-space occupancy and communicates this information to the web application, allowing the digital and physical states of the parking system to remain synchronized.

---

## Objectives

The project was designed to address several challenges associated with urban parking management.

### Main Objectives

* Reduce the time required to find an available parking space.
* Improve the management of available parking areas.
* Automate access to the parking facility.
* Provide users with updated parking information.
* Synchronize physical infrastructure with a web application.
* Experiment with embedded systems and web technologies.
* Develop a connected system combining electronics, software, databases, and communication protocols.
* Respect a maximum hardware budget of **€150**.

The original project requirements also identified automatic vehicle detection, parking availability information, access control, continuous operation, low energy consumption, and maintainability as major service functions.

---

## System Architecture

The system is organized into three main layers:

```text
┌──────────────────────────────────────────────────┐
│                    USER LAYER                    │
│                                                  │
│          Web Interface / Reservation System      │
└────────────────────────┬─────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────┐
│                APPLICATION LAYER                 │
│                                                  │
│       Symfony / REST API / Doctrine / MySQL      │
│                                                  │
│  • Users                                         │
│  • Reservations                                  │
│  • QR Codes                                      │
│  • Parking Spaces                                │
│  • Access History                                │
└────────────────────────┬─────────────────────────┘
                         │
                         │ JSON / API
                         ▼
┌──────────────────────────────────────────────────┐
│                 EMBEDDED LAYER                   │
│                                                  │
│          Arduino UNO R4 WiFi + ESP32-CAM         │
│                                                  │
│  • Ultrasonic Sensors                            │
│  • LEDs                                          │
│  • LCD Display                                   │
│  • Servo Motors                                  │
└────────────────────────┬─────────────────────────┘
                         │
                         ▼
                ┌─────────────────┐
                │ Physical Parking│
                └─────────────────┘
```

This architecture enables communication between the web platform and the physical prototype, allowing parking-space information to be transmitted and updated throughout the system.

---

## System Workflow

The general workflow is:

```text
User accesses the web application
              │
              ▼
Checks available parking spaces
              │
              ▼
Makes a reservation
              │
              ▼
System generates a unique QR code
              │
              ▼
User presents QR code at the entrance
              │
              ▼
ESP32-CAM reads the QR code
              │
              ▼
Symfony REST API validates the reservation
              │
        ┌─────┴─────┐
        │           │
      VALID       INVALID
        │           │
        ▼           ▼
 Open barrier   Access denied
        │
        ▼
Vehicle enters the parking
        │
        ▼
Sensor detects vehicle presence
        │
        ▼
Parking-space status is updated
        │
        ▼
Data is transmitted to the server
        │
        ▼
Web interface is updated
```

The objective is to maintain consistency between the physical state of the parking infrastructure and the information displayed on the web application.

---

## Web Application

The web application was developed using the **Symfony PHP framework** and follows an **MVC architecture**.

### Main Features

* Parking-space availability display.
* Online parking reservation.
* Automatic QR-code generation.
* User account management.
* Administration interface.
* Parking-space status management.
* Communication with the embedded infrastructure.
* Real-time synchronization between the physical model and the web platform.

The web application acts as the main digital interface between users and the smart parking system.

---

## Database

The project uses a relational database managed with **MySQL** and **Doctrine ORM**.

The database centralizes the information required for the operation of the system.

### Main Entities

```text
User
 │
 ├── Reservation
 │      │
 │      └── QR Code
 │
 └── Access History

Parking Space
 │
 └── Availability Status
```

### Main Data Categories

* Users
* Reservations
* Parking spaces
* QR codes
* Parking-space status
* Access history

For the academic prototype, fictitious data is used for demonstrations. No real banking information is intended to be stored.

---

## REST API

A **REST API developed with Symfony** provides communication between the web platform and the physical parking prototype.

The API uses **JSON** for data exchange.

### Main Responsibilities

* Validate QR codes.
* Retrieve parking-space status.
* Update parking-space occupancy.
* Receive events generated by the Arduino system.
* Synchronize physical events with the database.
* Provide updated information to the web application.

The API acts as the communication bridge between the embedded system and the application layer.

### Communication Flow

```text
Embedded System
       │
       │ HTTP / JSON
       ▼
Symfony REST API
       │
       ├── QR Code Validation
       │
       ├── Parking Status
       │
       ├── Occupancy Updates
       │
       └── Embedded Events
       │
       ▼
Database
       │
       ▼
Web Application
```

---

## Embedded System

The physical prototype is built around several electronic components.

| Component                      | Purpose                         |
| ------------------------------ | ------------------------------- |
| **Arduino UNO R4 WiFi**        | Main embedded controller        |
| **ESP32-CAM**                  | QR-code reading and recognition |
| **HC-SR04 ultrasonic sensors** | Vehicle presence detection      |
| **Infrared sensors**           | Entrance and exit detection     |
| **Servo motors**               | Automated parking barriers      |
| **LEDs**                       | Parking-space status indication |
| **LCD 16x2**                   | Parking information display     |
| **RGB LED strip**              | Parking model lighting          |
| **Breadboard**                 | Component integration           |
| **Jumper wires**               | Electrical connections          |

The embedded software is developed using **Arduino IDE and C++**.

---

## QR-Code Access Control

QR-code validation is one of the main access-control mechanisms implemented in the project.

```text
Web Reservation
       │
       ▼
Unique QR Code
       │
       ▼
ESP32-CAM
       │
       ▼
QR Code Recognition
       │
       ▼
Symfony REST API
       │
       ▼
Reservation Validation
       │
   ┌───┴────┐
   │        │
 VALID    INVALID
   │        │
   ▼        ▼
Open     Reject
Barrier  Access
   │
   ▼
Record Access
```

When a valid QR code is detected, the system authorizes access and triggers the opening of the parking barrier.

Invalid QR codes are rejected.

The access event can then be recorded in the database.

---

## Parking-Space Detection

Each parking space is equipped with a sensor designed to detect whether a vehicle is present.

When a vehicle is detected:

1. The physical state of the parking space changes.
2. The corresponding LED indicates the state of the space.
3. The embedded system processes the information.
4. The information is transmitted through the communication layer.
5. The database is updated.
6. The web interface can reflect the new parking status.

The tests conducted during the project demonstrated that the detection mechanism was sufficiently reliable for the scale of the prototype.

---

## 3D Modeling

Before the physical assembly of the prototype, the parking structure was modeled using **SolidWorks**.

The 3D modeling phase was used to design and organize:

* Parking spaces.
* Traffic lanes.
* Entrance and exit barriers.
* Sensor locations.
* Electronic component supports.
* Cable-management structures.
* Protective covers for electronic connections.

The covers were designed to improve the appearance of the prototype while protecting and organizing the wiring.

---

## Simulation and Testing

Several tools were used to validate the system before and during physical implementation.

### Simulation Tools

* **Wokwi**
* **Tinkercad**

These platforms were used to simulate electronic components and validate circuit behavior before working with the physical hardware.

### Physical Testing

The following components were tested:

* Ultrasonic sensors
* Infrared sensors
* LEDs
* Servo motors
* LCD display
* ESP32-CAM
* Arduino UNO R4 WiFi

### Functional Tests

| Test                          | Result     |
| ----------------------------- | ---------- |
| Vehicle detection             | Successful |
| Parking-space status update   | Successful |
| Valid QR-code recognition     | Successful |
| Invalid QR-code rejection     | Successful |
| Automatic barrier opening     | Successful |
| Automatic barrier closing     | Successful |
| Database synchronization      | Successful |
| Web interface synchronization | Successful |
| Web ↔ Embedded communication  | Functional |

The tests demonstrated that the main components of the prototype could communicate and operate together according to the project's objectives.

---

## Development Challenges

The development of **DAVINCI CUSTOM GARAGE** involved several technical challenges.

One of the main difficulties was implementing reliable **real-time parking-space display and synchronization** between the physical prototype and the web application.

### Symfony ↔ Arduino Synchronization

The communication between the Symfony application, REST API, and Arduino embedded system required several development and testing phases.

Particular difficulties were encountered in:

* Transmitting information between the web application and the embedded system.
* Synchronizing the actual state of parking spaces with the information displayed on the website.
* Updating parking availability in real time.
* Maintaining consistency between the database, Symfony, and Arduino.
* Handling events generated by physical sensors.
* Managing communication between multiple hardware and software components.

### Real-Time Parking Display

The expected communication chain was:

```text
Parking Sensors
       │
       ▼
Arduino
       │
       ▼
Communication Layer
       │
       ▼
Symfony REST API
       │
       ▼
Database
       │
       ▼
Web Interface
```

Maintaining synchronization throughout this entire chain proved to be one of the more challenging aspects of the project.

Although the prototype demonstrates the overall concept, this part of the system remains an important area for future improvement.

---

## Project Status

**DAVINCI CUSTOM GARAGE** is an evolving academic project.

The current version demonstrates the main concept of a connected smart parking system, but several technical aspects can still be improved.

### Planned Improvements

* Improve Symfony-to-Arduino communication.
* Optimize the REST API.
* Improve real-time data synchronization.
* Reduce synchronization delays.
* Improve code structure and maintainability.
* Improve database management.
* Strengthen system reliability.
* Improve UI/UX.
* Add new platform features.
* Improve the overall user experience.

The project should therefore be considered an **active prototype rather than a final production-ready solution**.

---

## Contributions and Suggestions

We are open to suggestions and ideas that could help improve the project.

We particularly welcome proposals related to:

* Code optimization
* Software architecture
* API design
* Symfony development
* Arduino communication
* Real-time data management
* Database optimization
* Cybersecurity
* UI/UX improvements
* User experience
* Embedded-system optimization
* New features and functionalities

If you have an idea that could improve the project, feel free to open an **Issue** or submit a **Pull Request**.

Any constructive contribution aimed at improving the code, reliability, architecture, security, or user experience is welcome.

---

## Budget

The project was developed under a strict hardware budget constraint of **€150**.

### Final Hardware Budget

| Category  |                    Amount |
| --------- | ------------------------: |
| **Total** | **€142.74 including VAT** |

The hardware components included:

* Arduino board
* ESP32-CAM
* Ultrasonic sensors
* Infrared sensors
* Servo motor
* LEDs
* LCD display
* Breadboard
* Jumper wires
* RGB LED strip

The project therefore remained within the defined budget constraint.

---

## Security and Privacy

Because this project was developed as part of a **Cybersecurity Bachelor's program**, security and data protection were considered during the design process.

### Security and Privacy Considerations

* Use of fictitious data for demonstrations.
* No real banking information.
* Consideration of GDPR requirements.
* Consideration of CNIL recommendations.
* Server-side validation of QR-code access.
* Controlled access to the physical parking.
* Centralized management of reservations and access information.

The project documentation also identifies electrical safety and applicable infrastructure regulations as important constraints for a real-world implementation.

---

## Current Limitations

Several limitations remain in the current prototype:

* Sensor accuracy can be affected by vehicle positioning and environmental conditions.
* The prototype operates at a small physical scale.
* QR-code recognition depends on lighting conditions and camera distance.
* The architecture has not yet been tested with a large number of simultaneous users.
* Real-world network availability requirements have not been fully addressed.
* The current real-time synchronization mechanism still requires optimization.
* Communication between the embedded system and the web application can be further improved.
* The current UI can be enhanced to provide a better user experience.

These limitations provide several opportunities for future development.

---

## Future Improvements

### Artificial Intelligence

AI-based algorithms could be integrated to predict parking occupancy and optimize the allocation of available spaces.

### Automatic License Plate Recognition

License-plate recognition could complement or eventually replace QR-code-based access control.

### Mobile Application

A dedicated mobile application could provide users with easier access to reservations and real-time parking availability.

### Cloud Infrastructure

Cloud-based services could improve scalability, availability, monitoring, and remote management.

### Automated Payments

A secure automated payment system could bring the prototype closer to a real-world commercial parking solution.

### Advanced Monitoring

Additional monitoring and analytics could be implemented to provide administrators with detailed information about parking usage and system performance.

---

## Repository Structure

The recommended repository structure is:

```text
davinci-custom-garage/
│
├── README.md
│
├── web/
│   ├── src/
│   ├── templates/
│   ├── config/
│   └── ...
│
├── api/
│   └── ...
│
├── embedded/
│   ├── arduino/
│   └── esp32-cam/
│
├── database/
│   └── ...
│
├── hardware/
│   ├── schematics/
│   └── components/
│
├── 3D/
│   └── solidworks/
│
├── simulation/
│   ├── wokwi/
│   └── tinkercad/
│
├── documentation/
│   ├── cahier-des-charges/
│   └── rapport/
│
└── assets/
    ├── images/
    └── diagrams/
```

---

## Technologies

### Software

* PHP
* Symfony
* Doctrine ORM
* MySQL
* SQL
* REST API
* JSON
* C++
* Arduino IDE
* Python

### Hardware

* Arduino UNO R4 WiFi
* ESP32-CAM
* HC-SR04 ultrasonic sensors
* Infrared sensors
* Servo motors
* LEDs
* LCD 16x2
* RGB LED strip
* Breadboard
* Jumper wires

### Design and Simulation

* SolidWorks
* Wokwi
* Tinkercad

---

## Skills Demonstrated

This project provided practical experience in several technical areas:

* Web application development
* PHP and Symfony development
* REST API development
* Relational database design
* SQL and Doctrine ORM
* Embedded programming
* C++ programming
* Arduino development
* ESP32 integration
* Sensor integration
* Electronic circuit design
* Real-time communication
* QR-code-based access control
* System integration
* 3D modeling
* Hardware simulation
* Technical testing
* Debugging and troubleshooting
* Team collaboration
* Project management

---

## Team

### Team 108 — ESILV

* **Kyllian Siewe Tiague**
* **Vikash Srikanthan**
* **Abdulrahman El Najjar**
* **Adam Tiss**

**Academic project developed as part of the Cybersecurity Bachelor's program at ESILV, 2025–2026.**

---

## Documentation

The project documentation includes:

* Project requirements specification
* Functional analysis
* Technical analysis
* Structural analysis
* Hardware component documentation
* Purchase order and budget documentation
* Web application documentation
* Embedded-system documentation
* Database documentation
* Testing and validation results
* Project development report

---

## Conclusion

**DAVINCI CUSTOM GARAGE** is an academic prototype demonstrating how **web technologies, embedded systems, electronics, databases, and connected infrastructure** can be combined to create a smart parking solution.

The project implements several key concepts:

* Online parking reservation
* QR-code-based access control
* Automated barriers
* Vehicle detection
* Parking-space monitoring
* REST API communication
* Database synchronization
* Web-to-hardware communication
* Real-time parking information

The project also provided practical experience in identifying and solving integration problems between software and hardware components.

Although several aspects of the system still require optimization — particularly **real-time display and synchronization between Symfony and the Arduino-based infrastructure** — the project provides a functional foundation for future development.

The team intends to continue improving the code, communication architecture, system reliability, and UI/UX.

**DAVINCI CUSTOM GARAGE is therefore not considered a finished product, but an evolving prototype open to optimization, experimentation, and future contributions.**

---

## Academic Project

**DAVINCI CUSTOM GARAGE — Smart Parking**

**ESILV — Cybersecurity Bachelor's Program**

**Academic Year 2025–2026**

**Team 108**

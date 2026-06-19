 **Hospital Management System**

A fully-featured desktop application for managing hospital operations, built with C++ and the Qt Framework. Designed with clean Object-Oriented Programming principles and a modern graphical user interface.


 **Table of Contents**

About
Features
Project Structure
OOP Concepts Used
Technologies Used
How to Run
Author



 **About**
The Hospital Management System is a desktop GUI application that streamlines core hospital operations including patient registration, doctor management, appointment scheduling, and billing. Data is persisted to local text files, allowing records to survive between sessions.


 **Features**

 **Patient Management**


Register new patients with ID, name, age, and phone number
View all registered patients
Search for a patient by ID
Remove patients from the system


**Doctor Management**


Add doctors with their details
View all available doctors
Search for a doctor by ID
Remove doctors from the system


 **Appointment Scheduling**


Schedule appointments between patients and doctors
Automatic slot conflict detection — prevents double-booking
Cancel or complete appointments
View all scheduled appointments


 **Billing System**


Generate itemized bills (consultation fee, medication fee, test fee)
Mark bills as paid
Auto-generate receipts for completed bills
View all billing records


**Data Persistence**


All records (patients, doctors, appointments, bills) are saved to .txt files
Data is automatically loaded on startup — no data loss between sessions



 **OOP Concepts Used**

ConceptWhere AppliedInheritancePatient and Doctor both inherit from PersonAbstractionPerson has a pure virtual method displayInfo()EncapsulationAll class attributes are private with getters/settersPolymorphismdisplayInfo() is overridden in each subclassCompositionHospitalSystem manages vectors of all entitiesSTL Containersvector, map, set used for data management


 **Technologies Used**


C++ — Core application logic
Qt Framework — GUI (Widgets, Signals & Slots)
Qt Creator — IDE
CMake / QMake — Build system
File I/O — Data persistence via .txt files



**How to Run**

Prerequisites


Qt Framework installed (Qt 5 or Qt 6)
Qt Creator IDE
A C++ compiler (MinGW / MSVC / GCC)


Steps


Clone the repository

bash   git clone https://github.com/waheedalidp/Hospital-Management-System.git

Open in Qt Creator
Launch Qt Creator
Click File → Open File or Project
Select hospital.pro
Build the project
Press Ctrl + B to build
Run the application

Press Ctrl + R to run



 Make sure the .txt data files (patients.txt, doctors.txt, etc.) are in the same directory as the executable so data loads correctly.



 Author

**Waheed Ali**


GitHub: @waheedalidp

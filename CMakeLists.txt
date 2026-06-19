cmake_minimum_required(VERSION 3.16)
project(HospitalManagementSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    main.cpp
    mainwindow.cpp
    Person.cpp
    Patient.cpp
    Doctor.cpp
    Appointment.cpp
    Bill.cpp
    HospitalSystem.cpp
    dashboardwidget.cpp
    patientdialog.cpp
    PatientPage.cpp
    DoctorPage.cpp
    AppointmentPage.cpp
    BillingPage.cpp
)

set(HEADERS
    mainwindow.h
    Person.h
    Patient.h
    Doctor.h
    Appointment.h
    Bill.h
    HospitalSystem.h
    dashboardwidget.h
    patientdialog.h
    PatientPage.h
    DoctorPage.h
    AppointmentPage.h
    BillingPage.h
)

add_executable(HospitalManagementSystem
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(HospitalManagementSystem
    Qt6::Widgets
)
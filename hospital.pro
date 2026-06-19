QT += core gui widgets

CONFIG += c++17

TARGET = HospitalGUI
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    Person.cpp \
    Patient.cpp \
    Doctor.cpp \
    Appointment.cpp \
    Bill.cpp \
    HospitalSystem.cpp \
    dashboardwidget.cpp \
    patientdialog.cpp \
    DoctorPage.cpp \
    AppointmentPage.cpp \
    BillingPage.cpp \
    Patientpage.cpp

HEADERS += \
    mainwindow.h \
    Person.h \
    Patient.h \
    Doctor.h \
    Appointment.h \
    Bill.h \
    HospitalSystem.h \
    dashboardwidget.h \
    patientdialog.h \
    PatientPage.h \
    DoctorPage.h \
    AppointmentPage.h \
    BillingPage.h
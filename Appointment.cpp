#include "Appointment.h"
#include <iostream>

Appointment::Appointment()
    : appointmentId(0), patientId(0), doctorId(0),
      date(""), time(""), status("Scheduled")
{
}

Appointment::Appointment(int appointmentId, int patientId, int doctorId,
                          const string &date, const string &time, const string &status)
    : appointmentId(appointmentId), patientId(patientId), doctorId(doctorId),
      date(date), time(time), status(status)
{
}

int Appointment::getAppointmentId() const
{
    return appointmentId;
}

int Appointment::getPatientId() const
{
    return patientId;
}

int Appointment::getDoctorId() const
{
    return doctorId;
}

string Appointment::getDate() const
{
    return date;
}

string Appointment::getTime() const
{
    return time;
}

string Appointment::getStatus() const
{
    return status;
}

void Appointment::setAppointmentId(int id)
{
    appointmentId = id;
}

void Appointment::setPatientId(int id)
{
    patientId = id;
}

void Appointment::setDoctorId(int id)
{
    doctorId = id;
}

void Appointment::setDate(const string &date)
{
    this->date = date;
}

void Appointment::setTime(const string &time)
{
    this->time = time;
}

void Appointment::setStatus(const string &status)
{
    this->status = status;
}

void Appointment::displayInfo() const
{
    std::cout << "Appointment ID : " << appointmentId << std::endl;
    std::cout << "Patient ID     : " << patientId << std::endl;
    std::cout << "Doctor ID      : " << doctorId << std::endl;
    std::cout << "Date           : " << date << std::endl;
    std::cout << "Time           : " << time << std::endl;
    std::cout << "Status         : " << status << std::endl;
}
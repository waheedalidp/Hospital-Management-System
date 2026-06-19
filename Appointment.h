#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
using namespace std;

class Appointment
{
private:
    int    appointmentId;
    int    patientId;
    int    doctorId;
    string date;
    string time;
    string status;

public:
    Appointment();

    Appointment(int appointmentId,
                int patientId,
                int doctorId,
                const string &date,
                const string &time,
                const string &status = "Scheduled");

    int    getAppointmentId() const;
    int    getPatientId()     const;
    int    getDoctorId()      const;
    string getDate()          const;
    string getTime()          const;
    string getStatus()        const;

    void setAppointmentId(int id);
    void setPatientId(int id);
    void setDoctorId(int id);
    void setDate(const string &date);
    void setTime(const string &time);
    void setStatus(const string &status);

    void displayInfo() const;
};

#endif
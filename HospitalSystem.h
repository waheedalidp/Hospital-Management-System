#ifndef HOSPITALSYSTEM_H
#define HOSPITALSYSTEM_H

#include "Patient.h"
#include "Doctor.h"
#include "Appointment.h"
#include "Bill.h"

#include <map>
#include <set>
#include <string>
#include <vector>

class HospitalSystem {
public:
    HospitalSystem();
    void saveAll();

    void     addPatient(Patient p);
    Patient* findPatient(int id);
    void     displayAllPatients();

    void    addDoctor(Doctor d);
    Doctor* findDoctor(int id);
    void    displayAllDoctors();

    void scheduleAppointment(int patId, int docId,
                             const std::string& date,
                             const std::string& time);
    void cancelAppointment(int appointmentId);
    void completeAppointment(int appointmentId);   
    void displayAllAppointments();

    void generateBill(int patientId,
                      double conFee, double medFee, double testFee);
    void markBillPaid(int billId);
    void displayAllBills();

    const std::vector<Patient>&     getPatients()     const { return patients; }
    const std::vector<Doctor>&      getDoctors()      const { return doctors; }
    const std::vector<Appointment>& getAppointments() const { return appointments; }
    const std::vector<Bill>&        getBills()        const { return bills; }

    void removePatient(int id);
    void removeDoctor(int id);   

private:
    std::vector<Patient>     patients;
    std::vector<Doctor>      doctors;
    std::vector<Appointment> appointments;
    std::vector<Bill>        bills;

    std::map<int, std::set<std::string>> bookedSlots;

    int nextPatientId     = 1;
    int nextDoctorId      = 1;
    int nextAppointmentId = 1;
    int nextBillId        = 1;

    void savePatients();     void loadPatients();
    void saveDoctors();      void loadDoctors();
    void saveAppointments(); void loadAppointments();
    void saveBills();        void loadBills();

    static std::string makeSlotKey(const std::string& date,
                                   const std::string& time);
    bool isSlotFree(int doctorId,
                    const std::string& date,
                    const std::string& time) const;
    void markSlotTaken(int doctorId,
                       const std::string& date,
                       const std::string& time);
    void markSlotFree(int doctorId,
                      const std::string& date,
                      const std::string& time);
    void rebuildSlots();
    void updateNextIds();

    static std::string currentDate();
    void writeReceipt(Bill& b) const;
};

#endif
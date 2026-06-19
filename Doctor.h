#ifndef DOCTOR_H
#define DOCTOR_H

#include "Person.h"
#include <vector>
#include <string>

using namespace std;

class Doctor : public Person
{
private:

    string specialization;
    vector<string> availableSlots;
    double consultationFee;

public:

    Doctor();

    Doctor(int id,
           string name,
           int age,
           string phone,
           string specialization,
           double consultationFee);

    string getSpecialization() const;
    vector<string> getAvailableSlots() const;
    double getConsultationFee() const;

    void setSpecialization(string specialization);
    void setAvailableSlots(vector<string> slots);
    void setConsultationFee(double fee);

    void displayInfo() override;

    bool isSlotAvailable(string time);

    void bookSlot(string time);
};

#endif
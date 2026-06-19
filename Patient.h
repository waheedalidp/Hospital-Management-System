#ifndef PATIENT_H
#define PATIENT_H

#include "Person.h"
#include <vector>

using namespace std;

class Patient : public Person
{
private:

    string disease;
    vector<string> medicalHistory;
    string admissionDate;

public:

    Patient();

    Patient(int id,
            string name,
            int age,
            string phone,
            string disease,
            string admissionDate);

    string getDisease() const;
    vector<string> getMedicalHistory() const;
    string getAdmissionDate() const;

    void setDisease(string disease);
    void setAdmissionDate(string admissionDate);

    void addToHistory(string note);

    void displayInfo() override;

    bool operator==(const Patient& other) const;
};

#endif
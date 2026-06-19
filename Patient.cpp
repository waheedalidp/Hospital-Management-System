#include "Patient.h"
#include <iostream>
#include <iomanip>   
#include <ctime>     

static std::string getCurrentDate()
{
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", local);
    return std::string(buffer);
}

Patient::Patient() : Person(), disease(""), admissionDate("")
{
    std::cout << "Patient created (default)" << std::endl;
}

Patient::Patient(int id, std::string name, int age, std::string phone,
                 std::string disease, std::string admissionDate)
    : Person(id, name, age, phone), disease(disease), admissionDate(admissionDate)
{
    std::cout << "Patient created: " << name << " (ID: " << id << ")" << std::endl;
}

std::string Patient::getDisease() const
{
    return disease;
}

std::vector<std::string> Patient::getMedicalHistory() const
{
    return medicalHistory;
}

std::string Patient::getAdmissionDate() const
{
    return admissionDate;
}

void Patient::setDisease(std::string disease)
{
    this->disease = disease;
}

void Patient::setAdmissionDate(std::string admissionDate)
{
    this->admissionDate = admissionDate;
}

void Patient::addToHistory(std::string note)
{
    std::string timestamp = getCurrentDate();
    std::string entry = "[" + timestamp + "] " + note;
    medicalHistory.push_back(entry);
    std::cout << "Medical history updated for patient " << getName() << std::endl;
}

void Patient::displayInfo()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "           PATIENT INFORMATION          " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "ID            : " << getId() << std::endl;
    std::cout << "Name          : " << getName() << std::endl;
    std::cout << "Age           : " << getAge() << std::endl;
    std::cout << "Phone         : " << getPhone() << std::endl;
    std::cout << "Disease       : " << disease << std::endl;
    std::cout << "Admission Date: " << admissionDate << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Medical History:" << std::endl;
    if (medicalHistory.empty())
    {
        std::cout << "  (No medical history recorded)" << std::endl;
    }
    else
    {
        for (size_t i = 0; i < medicalHistory.size(); ++i)
        {
            std::cout << "  " << (i + 1) << ". " << medicalHistory[i] << std::endl;
        }
    }
    std::cout << "========================================\n" << std::endl;
}

bool Patient::operator==(const Patient& other) const
{
    return (this->getId() == other.getId());
}
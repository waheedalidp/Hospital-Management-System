#include "Doctor.h"
#include <iostream>
#include <algorithm>  

Doctor::Doctor() : Person(), specialization(""), consultationFee(0.0)
{

    availableSlots = {"9:00AM", "10:00AM", "11:00AM", "2:00PM", "3:00PM", "4:00PM"};
    std::cout << "Doctor created (default)" << std::endl;
}

Doctor::Doctor(int id, std::string name, int age, std::string phone,
               std::string specialization, double consultationFee)
    : Person(id, name, age, phone), specialization(specialization), consultationFee(consultationFee)
{

    availableSlots = {"9:00AM", "10:00AM", "11:00AM", "2:00PM", "3:00PM", "4:00PM"};
    std::cout << "Doctor created: " << name << " (ID: " << id << ")" << std::endl;
}

std::string Doctor::getSpecialization() const
{
    return specialization;
}

std::vector<std::string> Doctor::getAvailableSlots() const
{
    return availableSlots;
}

double Doctor::getConsultationFee() const
{
    return consultationFee;
}

void Doctor::setSpecialization(std::string specialization)
{
    this->specialization = specialization;
}

void Doctor::setAvailableSlots(std::vector<std::string> slots)
{
    availableSlots = slots;
}

void Doctor::setConsultationFee(double fee)
{
    consultationFee = fee;
}

void Doctor::displayInfo()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "           DOCTOR INFORMATION           " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "ID            : " << getId() << std::endl;
    std::cout << "Name          : " << getName() << std::endl;
    std::cout << "Age           : " << getAge() << std::endl;
    std::cout << "Phone         : " << getPhone() << std::endl;
    std::cout << "Specialization: " << specialization << std::endl;
    std::cout << "Consultation Fee: $" << consultationFee << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Available Time Slots:" << std::endl;
    if (availableSlots.empty())
    {
        std::cout << "  (No slots available)" << std::endl;
    }
    else
    {
        for (size_t i = 0; i < availableSlots.size(); ++i)
        {
            std::cout << "  " << (i + 1) << ". " << availableSlots[i] << std::endl;
        }
    }
    std::cout << "========================================\n" << std::endl;
}

bool Doctor::isSlotAvailable(std::string time)
{
    auto it = std::find(availableSlots.begin(), availableSlots.end(), time);
    return (it != availableSlots.end());
}

void Doctor::bookSlot(std::string time)
{
    auto it = std::find(availableSlots.begin(), availableSlots.end(), time);
    if (it != availableSlots.end())
    {
        availableSlots.erase(it);
        std::cout << "Slot " << time << " booked successfully for Dr. " << getName() << std::endl;
    }
    else
    {
        std::cout << "Error: Slot " << time << " is not available for Dr. " << getName() << std::endl;
    }
}
#include "Bill.h"
#include <iostream>
#include <sstream>
#include <iomanip>

#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RESET   "\033[0m"

Bill::Bill()
    : billId(0), patientId(0), consultationFee(0.0), medicineFee(0.0),
      testFee(0.0), totalAmount(0.0), isPaid(false), billDate("")
{
    std::cout << "Bill created (default)" << std::endl;
}

Bill::Bill(int billId, int patientId, double consultationFee,
           double medicineFee, double testFee, std::string billDate)
    : billId(billId), patientId(patientId), consultationFee(consultationFee),
      medicineFee(medicineFee), testFee(testFee), totalAmount(0.0),
      isPaid(false), billDate(billDate)
{
    calculateTotal();  
    std::cout << "Bill created: ID " << billId << " for patient " << patientId << std::endl;
}

int Bill::getBillId() const
{
    return billId;
}

int Bill::getPatientId() const
{
    return patientId;
}

double Bill::getConsultationFee() const
{
    return consultationFee;
}

double Bill::getMedicineFee() const
{
    return medicineFee;
}

double Bill::getTestFee() const
{
    return testFee;
}

double Bill::getTotalAmount() const
{
    return totalAmount;
}

bool Bill::getIsPaid() const
{
    return isPaid;
}

std::string Bill::getBillDate() const
{
    return billDate;
}

void Bill::setBillId(int id)
{
    billId = id;
}

void Bill::setPatientId(int id)
{
    patientId = id;
}

void Bill::setConsultationFee(double fee)
{
    consultationFee = fee;
    calculateTotal();  
}

void Bill::setMedicineFee(double fee)
{
    medicineFee = fee;
    calculateTotal();
}

void Bill::setTestFee(double fee)
{
    testFee = fee;
    calculateTotal();
}

void Bill::setBillDate(std::string date)
{
    billDate = date;
}

void Bill::calculateTotal()
{
    totalAmount = consultationFee + medicineFee + testFee;
}

void Bill::displayBill()
{
    std::cout << "\n===================================" << std::endl;
    std::cout << "         HOSPITAL BILL             " << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Bill ID     : " << std::setw(4) << std::setfill('0') << billId << std::setfill(' ') << std::endl;
    std::cout << "Patient ID  : " << patientId << std::endl;
    std::cout << "Date        : " << billDate << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "Consultation: Rs. " << consultationFee << std::endl;
    std::cout << "Medicine    : Rs. " << medicineFee << std::endl;
    std::cout << "Tests       : Rs. " << testFee << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "TOTAL       : Rs. " << totalAmount << std::endl;
    std::cout << "Status      : ";
    if (isPaid)
        std::cout << COLOR_GREEN << "PAID" << COLOR_RESET << std::endl;
    else
        std::cout << COLOR_RED << "UNPAID" << COLOR_RESET << std::endl;
    std::cout << "===================================\n" << std::endl;
}

std::string Bill::generateReceipt()
{
    std::stringstream ss;
    ss << "\n===================================\n";
    ss << "         HOSPITAL BILL             \n";
    ss << "===================================\n";
    ss << "Bill ID     : " << std::setw(4) << std::setfill('0') << billId << std::setfill(' ') << "\n";
    ss << "Patient ID  : " << patientId << "\n";
    ss << "Date        : " << billDate << "\n";
    ss << "-----------------------------------\n";
    ss << "Consultation: Rs. " << consultationFee << "\n";
    ss << "Medicine    : Rs. " << medicineFee << "\n";
    ss << "Tests       : Rs. " << testFee << "\n";
    ss << "-----------------------------------\n";
    ss << "TOTAL       : Rs. " << totalAmount << "\n";
    ss << "Status      : ";
    if (isPaid)
        ss << "PAID\n";
    else
        ss << "UNPAID\n";
    ss << "===================================\n";
    return ss.str();
}

void Bill::markAsPaid()
{
    if (!isPaid)
    {
        isPaid = true;
        std::cout << "Payment confirmed for Bill #" << billId << std::endl;
    }
    else
    {
        std::cout << "Bill #" << billId << " is already paid." << std::endl;
    }
}
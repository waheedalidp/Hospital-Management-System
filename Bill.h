#ifndef BILL_H
#define BILL_H

#include <string>
using namespace std;

class Bill
{
private:
    int    billId;
    int    patientId;

    double consultationFee;
    double medicineFee;
    double testFee;
    double totalAmount;

    bool   isPaid;
    string billDate;

public:

    Bill();
    Bill(int billId,
         int patientId,
         double consultationFee,
         double medicineFee,
         double testFee,
         string billDate);

    int    getBillId()          const;
    int    getPatientId()       const;
    double getConsultationFee() const;
    double getMedicineFee()     const;
    double getTestFee()         const;
    double getTotalAmount()     const;
    bool   getIsPaid()          const;
    string getBillDate()        const;

    void setBillId(int id);
    void setPatientId(int id);
    void setConsultationFee(double fee);
    void setMedicineFee(double fee);
    void setTestFee(double fee);
    void setBillDate(string date);

    void calculateTotal();
    void markAsPaid();

    void        displayBill();
    string      generateReceipt();
};

#endif
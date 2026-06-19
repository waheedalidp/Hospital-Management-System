#include "HospitalSystem.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
    #include <direct.h>
    #define MAKE_DIR(p) _mkdir(p)
#else
    #include <sys/stat.h>
    #define MAKE_DIR(p) mkdir(p, 0755)
#endif

static const std::string FILE_PATIENTS     = "patients.txt";
static const std::string FILE_DOCTORS      = "doctors.txt";
static const std::string FILE_APPOINTMENTS = "appointments.txt";
static const std::string FILE_BILLS        = "bills.txt";
static const std::string DIR_RECEIPTS      = "receipts";

static void printLine(char c = '=', int w = 62) {
    std::cout << std::string(w, c) << '\n';
}
static void printHeader(const std::string& title) {
    printLine('=');
    int pad = (62 - (int)title.size()) / 2;
    if (pad > 0) std::cout << std::string(pad, ' ');
    std::cout << title << '\n';
    printLine('=');
}

static bool nextToken(std::istringstream& ss, std::string& out) {
    return static_cast<bool>(std::getline(ss, out, ','));
}

std::string HospitalSystem::makeSlotKey(const std::string& date,
                                        const std::string& time) {
    return date + "|" + time;
}

bool HospitalSystem::isSlotFree(int doctorId,
                                const std::string& date,
                                const std::string& time) const {
    auto it = bookedSlots.find(doctorId);
    if (it == bookedSlots.end()) return true;
    return it->second.count(makeSlotKey(date, time)) == 0;
}

void HospitalSystem::markSlotTaken(int doctorId,
                                   const std::string& date,
                                   const std::string& time) {
    bookedSlots[doctorId].insert(makeSlotKey(date, time));
}

void HospitalSystem::markSlotFree(int doctorId,
                                  const std::string& date,
                                  const std::string& time) {
    auto it = bookedSlots.find(doctorId);
    if (it != bookedSlots.end())
        it->second.erase(makeSlotKey(date, time));
}

void HospitalSystem::rebuildSlots() {
    bookedSlots.clear();
    for (const Appointment& a : appointments)
        if (a.getStatus() != "Cancelled")
            markSlotTaken(a.getDoctorId(), a.getDate(), a.getTime());
}

void HospitalSystem::updateNextIds() {
    for (const Patient& p : patients)
        if (p.getId() >= nextPatientId)
            nextPatientId = p.getId() + 1;

    for (const Doctor& d : doctors)
        if (d.getId() >= nextDoctorId)
            nextDoctorId = d.getId() + 1;

    for (const Appointment& a : appointments)
        if (a.getAppointmentId() >= nextAppointmentId)
            nextAppointmentId = a.getAppointmentId() + 1;

    for (const Bill& b : bills)
        if (b.getBillId() >= nextBillId)
            nextBillId = b.getBillId() + 1;
}

std::string HospitalSystem::currentDate() {
    std::time_t t = std::time(nullptr);
    std::tm* tm_ptr = std::localtime(&t);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_ptr);
    return std::string(buf);
}

void HospitalSystem::writeReceipt(Bill& b) const {
    MAKE_DIR(DIR_RECEIPTS.c_str());
    std::string path = DIR_RECEIPTS + "/receipt_"
                     + std::to_string(b.getBillId()) + ".txt";
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Cannot create receipt: " << path << '\n';
        return;
    }
    file << b.generateReceipt();   
    std::cout << "[OK] Receipt saved to '" << path << "'.\n";
}

void HospitalSystem::savePatients() {
    std::ofstream f(FILE_PATIENTS);
    if (!f.is_open()) {
        std::cerr << "[ERROR] Cannot write '" << FILE_PATIENTS << "'.\n";
        return;
    }
    for (const Patient& p : patients) {
        f << p.getId()            << ','
          << p.getName()          << ','
          << p.getAge()           << ','
          << p.getPhone()         << ','
          << p.getDisease()       << ','
          << p.getAdmissionDate() << '\n';
    }
    std::cout << "[OK] " << patients.size() << " patient(s) saved.\n";
}

void HospitalSystem::loadPatients() {
    std::ifstream f(FILE_PATIENTS);
    if (!f.is_open()) {
        std::cout << "[INFO] '" << FILE_PATIENTS
                  << "' not found — starting fresh.\n";
        return;
    }
    patients.clear();
    std::string line;
    int lineNo = 0;
    while (std::getline(f, line)) {
        ++lineNo;
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string tok;
        try {
            int id, age;
            std::string name, phone, disease, admDate;

            if (!nextToken(ss, tok))      throw std::runtime_error("id");
            id = std::stoi(tok);
            if (!nextToken(ss, name))     throw std::runtime_error("name");
            if (!nextToken(ss, tok))      throw std::runtime_error("age");
            age = std::stoi(tok);
            if (!nextToken(ss, phone))    throw std::runtime_error("phone");
            if (!nextToken(ss, disease))  throw std::runtime_error("disease");
            if (!nextToken(ss, admDate))  throw std::runtime_error("admissionDate");

            patients.emplace_back(id, name, age, phone, disease, admDate);
        } catch (const std::exception& e) {
            std::cerr << "[WARN] Bad patient record line "
                      << lineNo << " (" << e.what() << ") — skipped.\n";
        }
    }
    std::cout << "[OK] " << patients.size() << " patient(s) loaded.\n";
}

void HospitalSystem::saveDoctors() {
    std::ofstream f(FILE_DOCTORS);
    if (!f.is_open()) {
        std::cerr << "[ERROR] Cannot write '" << FILE_DOCTORS << "'.\n";
        return;
    }
    for (const Doctor& d : doctors) {
        f << d.getId()              << ','
          << d.getName()            << ','
          << d.getAge()             << ','
          << d.getPhone()           << ','
          << d.getSpecialization()  << ','
          << d.getConsultationFee() << '\n';
    }
    std::cout << "[OK] " << doctors.size() << " doctor(s) saved.\n";
}

void HospitalSystem::loadDoctors() {
    std::ifstream f(FILE_DOCTORS);
    if (!f.is_open()) {
        std::cout << "[INFO] '" << FILE_DOCTORS
                  << "' not found — starting fresh.\n";
        return;
    }
    doctors.clear();
    std::string line;
    int lineNo = 0;
    while (std::getline(f, line)) {
        ++lineNo;
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string tok;
        try {
            int id, age;
            double fee;
            std::string name, phone, spec;

            if (!nextToken(ss, tok))  throw std::runtime_error("id");
            id = std::stoi(tok);
            if (!nextToken(ss, name)) throw std::runtime_error("name");
            if (!nextToken(ss, tok))  throw std::runtime_error("age");
            age = std::stoi(tok);
            if (!nextToken(ss, phone))throw std::runtime_error("phone");
            if (!nextToken(ss, spec)) throw std::runtime_error("specialization");
            if (!nextToken(ss, tok))  throw std::runtime_error("consultationFee");
            fee = std::stod(tok);

            doctors.emplace_back(id, name, age, phone, spec, fee);
        } catch (const std::exception& e) {
            std::cerr << "[WARN] Bad doctor record line "
                      << lineNo << " (" << e.what() << ") — skipped.\n";
        }
    }
    std::cout << "[OK] " << doctors.size() << " doctor(s) loaded.\n";
}

void HospitalSystem::saveAppointments() {
    std::ofstream f(FILE_APPOINTMENTS);
    if (!f.is_open()) {
        std::cerr << "[ERROR] Cannot write '"
                  << FILE_APPOINTMENTS << "'.\n";
        return;
    }
    for (const Appointment& a : appointments) {
        f << a.getAppointmentId() << ','
          << a.getPatientId()     << ','
          << a.getDoctorId()      << ','
          << a.getDate()          << ','
          << a.getTime()          << ','
          << a.getStatus()        << '\n';
    }
    std::cout << "[OK] " << appointments.size()
              << " appointment(s) saved.\n";
}

void HospitalSystem::loadAppointments() {
    std::ifstream f(FILE_APPOINTMENTS);
    if (!f.is_open()) {
        std::cout << "[INFO] '" << FILE_APPOINTMENTS
                  << "' not found — starting fresh.\n";
        return;
    }
    appointments.clear();
    std::string line;
    int lineNo = 0;
    while (std::getline(f, line)) {
        ++lineNo;
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string tok;
        try {
            int aId, patId, docId;
            std::string date, time, status;

            if (!nextToken(ss, tok))    throw std::runtime_error("appointmentId");
            aId = std::stoi(tok);
            if (!nextToken(ss, tok))    throw std::runtime_error("patientId");
            patId = std::stoi(tok);
            if (!nextToken(ss, tok))    throw std::runtime_error("doctorId");
            docId = std::stoi(tok);
            if (!nextToken(ss, date))   throw std::runtime_error("date");
            if (!nextToken(ss, time))   throw std::runtime_error("time");
            if (!nextToken(ss, status)) throw std::runtime_error("status");

            if (status == "Cancelled" || status == "Completed") continue;
            appointments.emplace_back(aId, patId, docId, date, time, status);
        } catch (const std::exception& e) {
            std::cerr << "[WARN] Bad appointment record line "
                      << lineNo << " (" << e.what() << ") — skipped.\n";
        }
    }
    std::cout << "[OK] " << appointments.size()
              << " appointment(s) loaded.\n";
}

void HospitalSystem::saveBills() {
    std::ofstream f(FILE_BILLS);
    if (!f.is_open()) {
        std::cerr << "[ERROR] Cannot write '" << FILE_BILLS << "'.\n";
        return;
    }
    for (const Bill& b : bills) {
        f << b.getBillId()          << ','
          << b.getPatientId()       << ','
          << b.getConsultationFee() << ','
          << b.getMedicineFee()     << ','
          << b.getTestFee()         << ','
          << b.getIsPaid()          << ','   
          << b.getBillDate()        << '\n';
    }
    std::cout << "[OK] " << bills.size() << " bill(s) saved.\n";
}

void HospitalSystem::loadBills() {
    std::ifstream f(FILE_BILLS);
    if (!f.is_open()) {
        std::cout << "[INFO] '" << FILE_BILLS
                  << "' not found — starting fresh.\n";
        return;
    }
    bills.clear();
    std::string line;
    int lineNo = 0;
    while (std::getline(f, line)) {
        ++lineNo;
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string tok;
        try {
            int bId, patId;
            double conFee, medFee, testFee;
            bool paid;
            std::string date;

            if (!nextToken(ss, tok))  throw std::runtime_error("billId");
            bId = std::stoi(tok);
            if (!nextToken(ss, tok))  throw std::runtime_error("patientId");
            patId = std::stoi(tok);
            if (!nextToken(ss, tok))  throw std::runtime_error("consultationFee");
            conFee = std::stod(tok);
            if (!nextToken(ss, tok))  throw std::runtime_error("medicineFee");
            medFee = std::stod(tok);
            if (!nextToken(ss, tok))  throw std::runtime_error("testFee");
            testFee = std::stod(tok);
            if (!nextToken(ss, tok))  throw std::runtime_error("isPaid");
            paid = (std::stoi(tok) != 0);
            if (!nextToken(ss, date)) throw std::runtime_error("billDate");

            if (paid) continue;

            Bill b(bId, patId, conFee, medFee, testFee, date);
            bills.push_back(b);
        } catch (const std::exception& e) {
            std::cerr << "[WARN] Bad bill record line "
                      << lineNo << " (" << e.what() << ") — skipped.\n";
        }
    }
    std::cout << "[OK] " << bills.size() << " bill(s) loaded.\n";
}

HospitalSystem::HospitalSystem() {
    printHeader("Loading Hospital Data");
    loadPatients();
    loadDoctors();
    loadAppointments();
    loadBills();
    rebuildSlots();    
    updateNextIds();   
    printLine('=');
    std::cout << '\n';
}

void HospitalSystem::saveAll() {
    printHeader("Saving All Data");
    savePatients();
    saveDoctors();
    saveAppointments();
    saveBills();
    printLine('=');
    std::cout << '\n';
}

void HospitalSystem::addPatient(Patient p) {
    p.setId(nextPatientId++);          
    patients.push_back(p);
    savePatients();
    std::cout << "[OK] Patient '" << p.getName()
              << "' added with ID " << p.getId() << ".\n";
}

Patient* HospitalSystem::findPatient(int id) {
    for (Patient& p : patients)
        if (p.getId() == id) return &p;
    return nullptr;
}

void HospitalSystem::displayAllPatients() {
    printHeader("All Patients");
    if (patients.empty()) {
        std::cout << "  No patients on record.\n";
        printLine('=');
        return;
    }
    for (Patient& p : patients) p.displayInfo();   
    printLine('=');
    std::cout << "  Total: " << patients.size() << " patient(s).\n\n";
}

void HospitalSystem::addDoctor(Doctor d) {
    d.setId(nextDoctorId++);
    doctors.push_back(d);
    saveDoctors();
    std::cout << "[OK] Doctor '" << d.getName()
              << "' added with ID " << d.getId() << ".\n";
}

Doctor* HospitalSystem::findDoctor(int id) {
    for (Doctor& d : doctors)
        if (d.getId() == id) return &d;
    return nullptr;
}

void HospitalSystem::displayAllDoctors() {
    printHeader("All Doctors");
    if (doctors.empty()) {
        std::cout << "  No doctors on record.\n";
        printLine('=');
        return;
    }
    for (Doctor& d : doctors) d.displayInfo();   
    printLine('=');
    std::cout << "  Total: " << doctors.size() << " doctor(s).\n\n";
}

void HospitalSystem::scheduleAppointment(int patId, int docId,
                                         const std::string& date,
                                         const std::string& time) {

    if (!findPatient(patId)) {
        std::cerr << "[ERROR] Patient id=" << patId << " not found.\n";
        return;
    }

    Doctor* doc = findDoctor(docId);
    if (!doc) {
        std::cerr << "[ERROR] Doctor id=" << docId << " not found.\n";
        return;
    }

    if (!isSlotFree(docId, date, time)) {
        std::cerr << "[ERROR] Dr. " << doc->getName()
                  << " already has an appointment on "
                  << date << " at " << time
                  << ". Choose a different slot.\n";
        return;
    }

    Appointment a(nextAppointmentId++, patId, docId, date, time, "Scheduled");
    appointments.push_back(a);

    markSlotTaken(docId, date, time);

    saveAppointments();

    std::cout << "[OK] Appointment #" << a.getAppointmentId()
              << " scheduled with Dr. " << doc->getName()
              << " on " << date << " at " << time << ".\n";
}

void HospitalSystem::cancelAppointment(int appointmentId) {
    auto it = std::find_if(
        appointments.begin(), appointments.end(),
        [appointmentId](const Appointment& a) {
            return a.getAppointmentId() == appointmentId;
        });

    if (it == appointments.end()) {
        std::cerr << "[ERROR] Appointment id="
                  << appointmentId << " not found.\n";
        return;
    }
    markSlotFree(it->getDoctorId(), it->getDate(), it->getTime());
    it->setStatus("Cancelled");
    saveAppointments();

    std::cout << "[OK] Appointment #" << appointmentId << " cancelled.\n";
}

void HospitalSystem::displayAllAppointments() {
    printHeader("All Appointments");
    if (appointments.empty()) {
        std::cout << "  No appointments on record.\n";
        printLine('=');
        return;
    }
    for (Appointment& a : appointments)
        a.displayInfo();
    printLine('=');
    std::cout << "  Total: " << appointments.size()
              << " appointment(s).\n\n";
}

void HospitalSystem::generateBill(int patientId,
                                   double conFee,
                                   double medFee,
                                   double testFee) {
    Patient* pat = findPatient(patientId);
    if (!pat) {
        std::cerr << "[ERROR] Patient id=" << patientId << " not found.\n";
        return;
    }

    Bill b(nextBillId++, patientId, conFee, medFee, testFee, currentDate());
    bills.push_back(b);
    saveBills();

    std::cout << "[OK] Bill #" << b.getBillId()
              << " generated for patient '" << pat->getName()
              << "'. Total: PKR "
              << std::fixed << std::setprecision(2)
              << b.getTotalAmount() << " (UNPAID).\n";
}

void HospitalSystem::markBillPaid(int billId) {
    auto it = std::find_if(
        bills.begin(), bills.end(),
        [billId](const Bill& b) { return b.getBillId() == billId; });

    if (it == bills.end()) {
        std::cerr << "[ERROR] Bill id=" << billId << " not found.\n";
        return;
    }
    if (it->getIsPaid()) {
        std::cout << "[INFO] Bill #" << billId
                  << " is already paid.\n";
        return;
    }

    it->markAsPaid();    
    saveBills();
    writeReceipt(*it);

    std::cout << "[OK] Bill #" << billId << " marked PAID. PKR "
              << std::fixed << std::setprecision(2)
              << it->getTotalAmount() << " received.\n";
}

void HospitalSystem::displayAllBills() {
    printHeader("All Bills");
    if (bills.empty()) {
        std::cout << "  No bills on record.\n";
        printLine('=');
        return;
    }
    for (Bill& b : bills) b.displayBill();   
    printLine('=');
    std::cout << "  Total: " << bills.size() << " bill(s).\n\n";
}
void HospitalSystem::removePatient(int id)
{
    auto it = std::find_if(patients.begin(), patients.end(),
                           [id](const Patient& p){ return p.getId() == id; });

    if (it == patients.end()) {
        std::cerr << "[ERROR] removePatient: id=" << id << " not found.\n";
        return;
    }

    std::cout << "[OK] Patient '" << it->getName()
              << "' (id=" << id << ") removed.\n";

    patients.erase(it);
    savePatients();
}

void HospitalSystem::removeDoctor(int id)
{
    auto it = std::find_if(doctors.begin(), doctors.end(),
                           [id](const Doctor& d){ return d.getId() == id; });

    if (it == doctors.end()) {
        std::cerr << "[ERROR] removeDoctor: id=" << id << " not found.\n";
        return;
    }

    std::cout << "[OK] Doctor '" << it->getName()
              << "' (id=" << id << ") removed.\n";

    doctors.erase(it);
    saveDoctors();
}

void HospitalSystem::completeAppointment(int appointmentId)
{
    auto it = std::find_if(
        appointments.begin(), appointments.end(),
        [appointmentId](const Appointment& a) {
            return a.getAppointmentId() == appointmentId;
        });

    if (it == appointments.end()) {
        std::cerr << "[ERROR] Appointment id=" << appointmentId << " not found.\n";
        return;
    }
    if (it->getStatus() == "Completed") {
        std::cout << "[INFO] Appointment #" << appointmentId
                  << " is already completed.\n";
        return;
    }
    if (it->getStatus() == "Cancelled") {
        std::cerr << "[ERROR] Cannot complete a cancelled appointment.\n";
        return;
    }

    markSlotFree(it->getDoctorId(), it->getDate(), it->getTime());
    it->setStatus("Completed");
    saveAppointments();

    std::cout << "[OK] Appointment #" << appointmentId << " marked Completed.\n";
}
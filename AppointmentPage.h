#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QFormLayout>
#include <QFrame>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>

#include "HospitalSystem.h"

class ScheduleAppointmentDialog : public QDialog
{
    Q_OBJECT

public:

    explicit ScheduleAppointmentDialog(HospitalSystem *hs,
                                       QWidget *parent = nullptr);

    int     patientId()  const { return m_selectedPatientId; }
    int     doctorId()   const { return m_selectedDoctorId;  }
    QString date()       const;
    QString time()       const { return m_timeCombo->currentText(); }

private slots:
    void onDoctorChanged(int index);

private:
    void setupUi();
    void applyStyles();
    void populateTimeSlots(int doctorId);

    HospitalSystem  *m_hs;

    QComboBox   *m_patientCombo;
    QComboBox   *m_doctorCombo;
    QComboBox   *m_timeCombo;
    QDateEdit   *m_datePicker;
    QPushButton *m_scheduleBtn;
    QPushButton *m_cancelBtn;

    QVector<int> m_patientIds;
    QVector<int> m_doctorIds;

    int m_selectedPatientId = -1;
    int m_selectedDoctorId  = -1;
};

class AppointmentPage : public QWidget
{
    Q_OBJECT

public:

    explicit AppointmentPage(HospitalSystem *hs, QWidget *parent = nullptr);

    void refresh();

signals:
    void dataChanged();

private slots:
    void openScheduleDialog();
    void onCompleteClicked(int appointmentId);
    void onCancelClicked(int appointmentId);
    void applyFilter(int index);

private:
    void setupUi();
    void applyStyles();
    void populateRow(int row, const Appointment &appt);

    HospitalSystem  *m_hs;

    QPushButton  *m_scheduleBtn;
    QComboBox    *m_filterCombo;
    QTableWidget *m_table;
    QLabel       *m_summaryLabel;

    enum Col {
        COL_ID = 0,
        COL_PATIENT,
        COL_DOCTOR,
        COL_DATE,
        COL_TIME,
        COL_STATUS,
        COL_ACTION,
        COL_COUNT
    };
};
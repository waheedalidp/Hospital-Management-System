#pragma once

#include "HospitalSystem.h"

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVector>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QDate>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QFrame>
#include <QScrollArea>
#include <QTextStream>
#include <QFile>
#include <QButtonGroup>

struct BillRecord {
    int         billId;
    QString     patientName;
    double      consultationFee;
    double      medicineFee;
    double      testFee;
    double      total;
    bool        paid;
    QDate       date;
};

class GenerateBillDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenerateBillDialog(HospitalSystem *hs, QWidget *parent = nullptr);

    BillRecord  result() const;
    int         selectedPatientId() const { return m_selectedPatientId; }

private slots:
    void        recalcTotal();

private:
    void        setupUi();
    void        applyStyles();

    HospitalSystem  *m_hs;
    QVector<int>     m_patientIds;
    int              m_selectedPatientId = -1;

    QComboBox       *m_patientCombo;
    QDoubleSpinBox  *m_consultSpin;
    QDoubleSpinBox  *m_medicineSpin;
    QDoubleSpinBox  *m_testSpin;
    QLabel          *m_totalLabel;
    QDateEdit       *m_datePicker;
    QPushButton     *m_generateBtn;
    QPushButton     *m_cancelBtn;
};

class ReceiptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReceiptDialog(const BillRecord &bill, QWidget *parent = nullptr);

private slots:
    void        saveReceipt();

private:
    void        setupUi();
    void        applyStyles();
    QString     buildReceiptText() const;

    BillRecord  m_bill;
    QTextEdit   *m_receiptView;
    QPushButton *m_saveBtn;
    QPushButton *m_closeBtn;
};

class BillingPage : public QWidget
{
    Q_OBJECT

public:
    explicit BillingPage(HospitalSystem *hs, QWidget *parent = nullptr);

    void refresh();

signals:
    void        dataChanged();

private slots:
    void        openGenerateBillDialog();
    void        applyFilter(int index);
    void        markPaid(int billId);
    void        viewReceipt(int billId);

private:
    void        setupUi();
    void        applyStyles();

    void        refreshTable();
    void        populateRow(int row, const BillRecord &bill);
    BillRecord *findBill(int billId);

    HospitalSystem      *m_hs;

    QPushButton     *m_generateBtn;
    QComboBox       *m_filterCombo;
    QTableWidget    *m_table;
    QLabel          *m_summaryLabel;

    QVector<BillRecord> m_bills;

    enum Col {
        COL_ID = 0,
        COL_PATIENT,
        COL_CONSULT,
        COL_MEDICINE,
        COL_TESTS,
        COL_TOTAL,
        COL_STATUS,
        COL_ACTION,
        COL_COUNT
    };
};
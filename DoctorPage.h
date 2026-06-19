#ifndef DOCTORPAGE_H
#define DOCTORPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QRegularExpression>

#include "HospitalSystem.h"

struct DoctorRecord {
    int     id   = 0;
    QString name;
    int     age  = 30;
    QString phone;
    QString specialization;
    double  consultationFee = 0.0;
};

class DoctorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DoctorDialog(QWidget *parent = nullptr);

    DoctorRecord getData() const;

private slots:
    void onSaveClicked();

private:
    void buildUi();
    void applyStyles();
    bool validateAll();

    QLineEdit      *nameEdit_;
    QSpinBox       *ageSpin_;
    QLineEdit      *phoneEdit_;
    QLineEdit      *specEdit_;
    QDoubleSpinBox *feeSpin_;
    QLabel         *errorLabel_;
    QPushButton    *saveBtn_;
    QPushButton    *cancelBtn_;
};

class DoctorPage : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorPage(HospitalSystem *hs, QWidget *parent = nullptr);

    void refreshTable();

signals:
    void dataChanged();

private slots:
    void onAddClicked();
    void onDeleteClicked();

private:
    void buildUi();
    void applyStyles();

    HospitalSystem *hs_;
    QTableWidget   *table_;
    QPushButton    *addBtn_;
    QLabel         *countLabel_;
};

#endif
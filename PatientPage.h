#ifndef PATIENTPAGE_H
#define PATIENTPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

#include "HospitalSystem.h"
#include "patientdialog.h"

class PatientPage : public QWidget
{
    Q_OBJECT

public:

    explicit PatientPage(HospitalSystem *hs, QWidget *parent = nullptr);

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
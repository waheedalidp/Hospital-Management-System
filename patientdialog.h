#ifndef PATIENTDIALOG_H
#define PATIENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

struct PatientRecord {
    QString id;             
    QString name;
    int     age       = 0;
    QString disease;
    QString phone;
    QString admissionDate;  
};

class ValidatedLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit ValidatedLineEdit(const QString &placeholder, QWidget *parent = nullptr);
    bool   validate();           
    void   clearError();

private:
    QString baseStyle_;
};

class PatientDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode { AddMode, EditMode };

    explicit PatientDialog(Mode mode, QWidget *parent = nullptr);

    void            setPatientData(const PatientRecord &record);

    PatientRecord   getPatientData() const;

private slots:
    void onSaveClicked();

private:
    Mode              mode_;

    ValidatedLineEdit *nameEdit_;
    QSpinBox          *ageSpinBox_;
    ValidatedLineEdit *phoneEdit_;
    ValidatedLineEdit *diseaseEdit_;
    QDateEdit         *admissionDateEdit_;

    QPushButton       *saveBtn_;
    QPushButton       *cancelBtn_;

    QLabel            *errorLabel_;

    void buildUi();
    void applyStyles();
    bool validateAll();

    QString editingId_;
};

#endif
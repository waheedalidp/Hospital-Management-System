#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QVector>
#include "HospitalSystem.h"
#include "PatientPage.h"
#include "DoctorPage.h"          
#include "AppointmentPage.h"     
#include "BillingPage.h"         
#include "dashboardwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void refreshDashboard();   

private slots:
    void onSidebarButtonClicked(int index);

private:

    QWidget        *centralWidget;
    QVBoxLayout    *rootLayout;
    QHBoxLayout    *bodyLayout;

    QWidget        *topBar;
    QHBoxLayout    *topBarLayout;
    QLabel         *titleLabel;

    QWidget        *sidebar;
    QVBoxLayout    *sidebarLayout;
    QLabel         *logoLabel;
    QVector<QPushButton*> sidebarButtons;

    QStackedWidget *stackedWidget;

    DashboardWidget *dashboardWidget_ = nullptr;   

    void setupTopBar();
    void setupSidebar();
    void setupStackedWidget();
    void setupStyles();

    QPushButton *createSidebarButton(const QString &text, const QString &iconSymbol);
    QWidget     *createPage(const QString &pageName, const QString &color);
    QVector<AppointmentRecord> buildAppointmentRecords();  

    int currentIndex = 0;
    void updateButtonStyles(int activeIndex);

    HospitalSystem   hs_;
    PatientPage     *patientPage_      = nullptr;
    DoctorPage      *doctorPage_       = nullptr;   
    AppointmentPage *appointmentPage_  = nullptr;   
    BillingPage     *billingPage_      = nullptr;   
};

#endif
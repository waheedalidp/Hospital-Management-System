#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QScrollArea>
#include <QFrame>
#include <QVector>

struct AppointmentRecord {
    QString id;
    QString patient;
    QString doctor;
    QString time;
    QString status;   
};

class StatCard : public QFrame
{
    Q_OBJECT

public:
    explicit StatCard(const QString &icon,
                      const QString &label,
                      const QString &accentColor,
                      const QString &lightColor,
                      QWidget *parent = nullptr);

    void setValue(int value);
    void setValue(const QString &value);

private:
    QLabel *iconLabel_;
    QLabel *valueLabel_;
    QLabel *textLabel_;
    QLabel *trendLabel_;

    void buildUi(const QString &icon,
                 const QString &label,
                 const QString &accentColor,
                 const QString &lightColor);
};

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget *parent = nullptr);

    void setTotalPatients(int count);
    void setTotalDoctors(int count);
    void setTodayAppointments(int count);
    void setUnpaidBills(int count);

    void setAppointments(const QVector<AppointmentRecord> &records);
    void addAppointment(const AppointmentRecord &record);
    void clearAppointments();

    void updateStats(int patients, int doctors, int appointments, int unpaidBills);

private:

    StatCard *cardPatients_;
    StatCard *cardDoctors_;
    StatCard *cardAppointments_;
    StatCard *cardBills_;

    QTableWidget *appointmentTable_;

    QLabel *sectionHeader_;
    QLabel *subHeader_;

    void buildUi();
    void buildStatsRow(QHBoxLayout *row);
    void buildAppointmentsSection(QVBoxLayout *parent);
    void styleTable();

    static QWidget *makeStatusBadge(const QString &status);
    static QString statusColor(const QString &status);
    static QString statusBg(const QString &status);
};

#endif
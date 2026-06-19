#include "dashboardwidget.h"

#include <QDateTime>
#include <QScrollArea>
#include <QScrollBar>
#include <QSizePolicy>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QPalette>

StatCard::StatCard(const QString &icon,
                   const QString &label,
                   const QString &accentColor,
                   const QString &lightColor,
                   QWidget *parent)
    : QFrame(parent)
{
    buildUi(icon, label, accentColor, lightColor);
}

void StatCard::buildUi(const QString &icon,
                       const QString &label,
                       const QString &accentColor,
                       const QString &lightColor)
{
    setObjectName("statCard");
    setFixedHeight(130);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    setStyleSheet(QString(
        "QFrame#statCard {"
        "  background-color: #FFFFFF;"
        "  border-radius: 14px;"
        "  border: 1.5px solid #E2E8F0;"
        "  border-left: 5px solid %1;"
        "}"
    ).arg(accentColor));

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(12);
    shadow->setOffset(0, 2);
    shadow->setColor(QColor(0, 0, 0, 18));
    setGraphicsEffect(shadow);

    auto *outerH = new QHBoxLayout(this);
    outerH->setContentsMargins(20, 16, 20, 16);
    outerH->setSpacing(16);

    iconLabel_ = new QLabel(icon);
    iconLabel_->setObjectName("cardIcon");
    iconLabel_->setAlignment(Qt::AlignCenter);
    iconLabel_->setFixedSize(56, 56);
    iconLabel_->setStyleSheet(QString(
        "QLabel {"
        "  background-color: %1;"
        "  border-radius: 28px;"
        "  font-size: 24px;"
        "}"
    ).arg(lightColor));

    auto *textV = new QVBoxLayout();
    textV->setSpacing(4);
    textV->setContentsMargins(0, 0, 0, 0);

    valueLabel_ = new QLabel("0");
    valueLabel_->setObjectName("cardValue");
    valueLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    valueLabel_->setStyleSheet(QString(
        "QLabel { color: #1A202C; font-size: 32px; font-weight: 800; "
        "         letter-spacing: -0.5px; }"
    ));

    textLabel_ = new QLabel(label);
    textLabel_->setObjectName("cardLabel");
    textLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    textLabel_->setStyleSheet(
        "QLabel { color: #718096; font-size: 13px; font-weight: 500; }"
    );

    trendLabel_ = new QLabel();
    trendLabel_->setObjectName("cardTrend");
    trendLabel_->setAlignment(Qt::AlignLeft);
    trendLabel_->setStyleSheet(
        QString("QLabel { color: %1; font-size: 11px; font-weight: 600; }").arg(accentColor)
    );
    trendLabel_->setVisible(false);

    textV->addStretch();
    textV->addWidget(valueLabel_);
    textV->addWidget(textLabel_);
    textV->addWidget(trendLabel_);
    textV->addStretch();

    outerH->addWidget(iconLabel_);
    outerH->addLayout(textV, 1);
}

void StatCard::setValue(int value)
{
    valueLabel_->setText(QString::number(value));
}

void StatCard::setValue(const QString &value)
{
    valueLabel_->setText(value);
}

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
}

void DashboardWidget::setTotalPatients(int count)     { cardPatients_->setValue(count); }
void DashboardWidget::setTotalDoctors(int count)      { cardDoctors_->setValue(count); }
void DashboardWidget::setTodayAppointments(int count) { cardAppointments_->setValue(count); }
void DashboardWidget::setUnpaidBills(int count)       { cardBills_->setValue(count); }

void DashboardWidget::updateStats(int patients, int doctors,
                                   int appointments, int unpaidBills)
{
    setTotalPatients(patients);
    setTotalDoctors(doctors);
    setTodayAppointments(appointments);
    setUnpaidBills(unpaidBills);
}

void DashboardWidget::clearAppointments()
{
    appointmentTable_->setRowCount(0);
}

void DashboardWidget::addAppointment(const AppointmentRecord &rec)
{
    int row = appointmentTable_->rowCount();
    appointmentTable_->insertRow(row);

    auto makeItem = [](const QString &text, Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft) {
        QTableWidgetItem *item = new QTableWidgetItem(text);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setTextAlignment(align);
        return item;
    };

    appointmentTable_->setItem(row, 0, makeItem(rec.id,      Qt::AlignCenter));
    appointmentTable_->setItem(row, 1, makeItem(rec.patient));
    appointmentTable_->setItem(row, 2, makeItem(rec.doctor));
    appointmentTable_->setItem(row, 3, makeItem(rec.time, Qt::AlignCenter));

    QWidget *badgeContainer = new QWidget();
    QHBoxLayout *bl = new QHBoxLayout(badgeContainer);
    bl->setContentsMargins(6, 4, 6, 4);
    bl->setAlignment(Qt::AlignCenter);
    bl->addWidget(makeStatusBadge(rec.status));
    appointmentTable_->setCellWidget(row, 4, badgeContainer);
}

void DashboardWidget::setAppointments(const QVector<AppointmentRecord> &records)
{
    clearAppointments();
    for (const auto &r : records) addAppointment(r);

    int scheduledCount = 0;
    for (const auto &r : records)
        if (r.status == "Scheduled") scheduledCount++;

    sectionHeader_->setText(
        QString("Today's Appointments  ·  %1 scheduled").arg(scheduledCount)
    );
}

void DashboardWidget::buildUi()
{
    setObjectName("dashboardWidget");

    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical { background: #F0F4F8; width: 8px; }"
        "QScrollBar::handle:vertical { background: #CBD5E0; border-radius: 4px; min-height: 20px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
    );

    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scroll);

    auto *inner = new QWidget();
    inner->setStyleSheet("background: transparent;");
    scroll->setWidget(inner);

    auto *mainLayout = new QVBoxLayout(inner);
    mainLayout->setContentsMargins(28, 24, 28, 32);
    mainLayout->setSpacing(0);

    auto *pageTitle = new QLabel("Dashboard");
    pageTitle->setStyleSheet(
        "color: #1A202C; font-size: 24px; font-weight: 800; letter-spacing: -0.3px;"
    );

    subHeader_ = new QLabel(
        "Welcome back  ·  " + QDateTime::currentDateTime().toString("dddd, MMMM d yyyy")
    );
    subHeader_->setStyleSheet("color: #718096; font-size: 13px; margin-top: 2px;");

    mainLayout->addWidget(pageTitle);
    mainLayout->addWidget(subHeader_);
    mainLayout->addSpacing(24);

    auto *statsRow = new QHBoxLayout();
    statsRow->setSpacing(16);
    buildStatsRow(statsRow);
    mainLayout->addLayout(statsRow);
    mainLayout->addSpacing(28);

    buildAppointmentsSection(mainLayout);
}

void DashboardWidget::buildStatsRow(QHBoxLayout *row)
{
    cardPatients_ = new StatCard(
        "🧑‍⚕️", "Total Patients",      "#3B82F6", "#EBF5FF", this);

    cardDoctors_ = new StatCard(
        "👨‍⚕️", "Total Doctors",        "#10B981", "#ECFDF5", this);

    cardAppointments_ = new StatCard(
        "📅", "Today's Appointments", "#F59E0B", "#FFFBEB", this);

    cardBills_ = new StatCard(
        "💳", "Unpaid Bills",         "#EF4444", "#FEF2F2", this);

    row->addWidget(cardPatients_);
    row->addWidget(cardDoctors_);
    row->addWidget(cardAppointments_);
    row->addWidget(cardBills_);
}

void DashboardWidget::buildAppointmentsSection(QVBoxLayout *parent)
{
    auto *headerRow = new QHBoxLayout();
    headerRow->setContentsMargins(0, 0, 0, 0);

    sectionHeader_ = new QLabel("Today's Appointments");
    sectionHeader_->setStyleSheet(
        "color: #1A202C; font-size: 17px; font-weight: 700;"
    );

    auto *refreshHint = new QLabel("Auto-refreshes with live data");
    refreshHint->setStyleSheet("color: #A0AEC0; font-size: 12px;");

    headerRow->addWidget(sectionHeader_);
    headerRow->addStretch();
    headerRow->addWidget(refreshHint);
    parent->addLayout(headerRow);
    parent->addSpacing(12);

    auto *tableCard = new QFrame();
    tableCard->setObjectName("tableCard");
    tableCard->setStyleSheet(
        "QFrame#tableCard {"
        "  background: #FFFFFF;"
        "  border-radius: 12px;"
        "}"
    );

    auto *shadow = new QGraphicsDropShadowEffect(tableCard);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 22));
    tableCard->setGraphicsEffect(shadow);

    auto *cardLayout = new QVBoxLayout(tableCard);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    appointmentTable_ = new QTableWidget(0, 5, tableCard);
    appointmentTable_->setHorizontalHeaderLabels(
        { "ID", "Patient", "Doctor", "Time", "Status" }
    );
    styleTable();
    cardLayout->addWidget(appointmentTable_);

    parent->addWidget(tableCard);
}

void DashboardWidget::styleTable()
{
    QTableWidget *t = appointmentTable_;

    t->setObjectName("appointmentTable");
    t->setShowGrid(false);
    t->setAlternatingRowColors(true);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode(QAbstractItemView::SingleSelection);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setFocusPolicy(Qt::NoFocus);
    t->verticalScrollBar()->setVisible(false);
    t->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    t->setMinimumHeight(120);

    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    t->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    t->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    t->setColumnWidth(0, 90);
    t->setColumnWidth(3, 90);
    t->setColumnWidth(4, 120);

    t->verticalHeader()->setVisible(false);
    t->verticalHeader()->setDefaultSectionSize(52);

    t->setStyleSheet(R"(
        QTableWidget#appointmentTable {
            background-color: #FFFFFF;
            border: none;
            border-radius: 12px;
            gridline-color: transparent;
            font-size: 13px;
            color: #2D3748;
            outline: none;
        }
        QTableWidget#appointmentTable::item {
            padding: 0 12px;
            border-bottom: 1px solid #F7FAFC;
        }
        QTableWidget#appointmentTable::item:selected {
            background-color: #EBF5FF;
            color: #1A202C;
        }
        QTableWidget#appointmentTable::item:alternate {
            background-color: #FAFBFC;
        }
        QHeaderView::section {
            background-color: #F7FAFC;
            color: #718096;
            font-size: 11px;
            font-weight: 700;
            text-transform: uppercase;
            letter-spacing: 0.8px;
            padding: 0 12px;
            height: 40px;
            border: none;
            border-bottom: 2px solid #EDF2F7;
        }
        QHeaderView::section:first {
            border-top-left-radius: 12px;
        }
        QHeaderView::section:last {
            border-top-right-radius: 12px;
        }
        QScrollBar:horizontal { height: 0px; }
    )");
}

QString DashboardWidget::statusColor(const QString &status)
{
    if (status == "Confirmed")  return "#065F46";
    if (status == "Pending")    return "#92400E";
    if (status == "Completed")  return "#1E40AF";
    if (status == "Cancelled")  return "#991B1B";
    if (status == "Scheduled")  return "#065F46";
    return "#374151";
}

QString DashboardWidget::statusBg(const QString &status)
{
    if (status == "Confirmed")  return "#D1FAE5";
    if (status == "Pending")    return "#FEF3C7";
    if (status == "Completed")  return "#DBEAFE";
    if (status == "Cancelled")  return "#FEE2E2";
    if (status == "Scheduled")  return "#D1FAE5";
    return "#F3F4F6";
}

QWidget *DashboardWidget::makeStatusBadge(const QString &status)
{
    QLabel *badge = new QLabel(status);
    badge->setAlignment(Qt::AlignCenter);
    badge->setFixedHeight(26);
    badge->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    badge->setStyleSheet(QString(
        "QLabel {"
        "  background-color: %1;"
        "  color: %2;"
        "  border-radius: 13px;"
        "  padding: 0 12px;"
        "  font-size: 11px;"
        "  font-weight: 700;"
        "  letter-spacing: 0.4px;"
        "}"
    ).arg(statusBg(status), statusColor(status)));
    return badge;
}
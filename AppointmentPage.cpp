#include "AppointmentPage.h"
#include <QApplication>

namespace APal {
    const QString BG_PAGE      = "#F0F4F8";
    const QString BG_CARD      = "#FFFFFF";
    const QString ACCENT       = "#1A6FC4";
    const QString ACCENT_HOVER = "#155AA0";
    const QString ACCENT_LIGHT = "#E8F1FB";
    const QString SUCCESS      = "#1B7A48";
    const QString SUCCESS_BG   = "#E6F4EC";
    const QString WARNING      = "#B7791F";
    const QString WARNING_BG   = "#FEFCBF";
    const QString DANGER       = "#C0392B";
    const QString DANGER_BG    = "#FDECEA";
    const QString TEXT_PRIMARY = "#1A202C";
    const QString TEXT_MUTED   = "#718096";
    const QString BORDER       = "#CBD5E0";
    const QString HEADER_BG    = "#1A3557";
    const QString HEADER_FG    = "#FFFFFF";
}

ScheduleAppointmentDialog::ScheduleAppointmentDialog(HospitalSystem *hs,
                                                     QWidget *parent)
    : QDialog(parent), m_hs(hs)
{
    setWindowTitle("Schedule Appointment");
    setModal(true);
    setMinimumWidth(420);
    setupUi();
    applyStyles();
}

void ScheduleAppointmentDialog::setupUi()
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(0);
    root->setContentsMargins(0, 0, 0, 0);

    auto *header = new QFrame;
    header->setObjectName("dialogHeader");
    header->setFixedHeight(60);
    header->setAutoFillBackground(true);
    auto *hLay = new QHBoxLayout(header);
    hLay->setContentsMargins(24, 0, 24, 0);
    auto *icon  = new QLabel("📅");
    icon->setStyleSheet("font-size:22px; background:transparent; color:#FFFFFF;");
    auto *title = new QLabel("Schedule Appointment");
    title->setObjectName("dialogTitle");
    title->setStyleSheet("color:#FFFFFF; font-size:15px; font-weight:700; background:transparent;");
    hLay->addWidget(icon);
    hLay->addSpacing(10);
    hLay->addWidget(title);
    hLay->addStretch();
    root->addWidget(header);

    auto *body = new QWidget;
    body->setObjectName("dialogBody");
    auto *form = new QFormLayout(body);
    form->setSpacing(14);
    form->setContentsMargins(28, 22, 28, 18);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_patientCombo = new QComboBox;
    m_patientCombo->setObjectName("formControl");
    for (const Patient &p : m_hs->getPatients()) {
        m_patientCombo->addItem(
            QString("%1 (ID %2)").arg(QString::fromStdString(p.getName())).arg(p.getId()));
        m_patientIds.append(p.getId());
    }
    form->addRow("Patient :", m_patientCombo);

    m_doctorCombo = new QComboBox;
    m_doctorCombo->setObjectName("formControl");
    for (const Doctor &d : m_hs->getDoctors()) {
        m_doctorCombo->addItem(
            QString("Dr. %1 — %2")
                .arg(QString::fromStdString(d.getName()))
                .arg(QString::fromStdString(d.getSpecialization())));
        m_doctorIds.append(d.getId());
    }
    form->addRow("Doctor :", m_doctorCombo);

    m_datePicker = new QDateEdit(QDate::currentDate());
    m_datePicker->setObjectName("formControl");
    m_datePicker->setCalendarPopup(true);
    m_datePicker->setDisplayFormat("dd MMM yyyy");
    m_datePicker->setMinimumDate(QDate::currentDate());
    form->addRow("Date :", m_datePicker);

    m_timeCombo = new QComboBox;
    m_timeCombo->setObjectName("formControl");
    if (!m_doctorIds.isEmpty())
        populateTimeSlots(m_doctorIds.first());
    form->addRow("Time Slot :", m_timeCombo);

    root->addWidget(body);

    auto *btnBar = new QWidget;
    btnBar->setObjectName("dialogBody");
    auto *btnLay = new QHBoxLayout(btnBar);
    btnLay->setContentsMargins(28, 10, 28, 18);
    btnLay->setSpacing(12);

    m_cancelBtn  = new QPushButton("Cancel");
    m_cancelBtn->setObjectName("cancelBtn");
    m_cancelBtn->setFixedHeight(40);

    m_scheduleBtn = new QPushButton("  Schedule");
    m_scheduleBtn->setObjectName("primaryBtn");
    m_scheduleBtn->setFixedHeight(40);

    btnLay->addStretch();
    btnLay->addWidget(m_cancelBtn);
    btnLay->addWidget(m_scheduleBtn);
    root->addWidget(btnBar);

    connect(m_datePicker, &QDateEdit::dateChanged, this, [this](const QDate &) {
        int idx = m_doctorCombo->currentIndex();
        if (idx >= 0 && idx < m_doctorIds.size())
            populateTimeSlots(m_doctorIds[idx]);
    });

    connect(m_doctorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ScheduleAppointmentDialog::onDoctorChanged);

    connect(m_scheduleBtn, &QPushButton::clicked, this, [this]() {
        if (m_patientIds.isEmpty() || m_doctorIds.isEmpty()) {
            QMessageBox::warning(this, "No Data",
                "No patients or doctors are registered yet.");
            return;
        }

        if (m_datePicker->date() < QDate::currentDate()) {
            QMessageBox::warning(this, "Invalid Date",
                "Appointment date cannot be in the past.\n"
                "Please select today or a future date.");
            return;
        }

        if (m_timeCombo->count() == 0 ||
            m_timeCombo->currentText() == "No slots available") {
            QMessageBox::warning(this, "No Slots Available",
                "This doctor has no available time slots on the selected date.\n"
                "Please choose a different date or doctor.");
            return;
        }
        m_selectedPatientId = m_patientIds.value(m_patientCombo->currentIndex(), -1);
        m_selectedDoctorId  = m_doctorIds.value(m_doctorCombo->currentIndex(), -1);
        accept();
    });
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void ScheduleAppointmentDialog::onDoctorChanged(int index)
{
    if (index < 0 || index >= m_doctorIds.size()) return;
    populateTimeSlots(m_doctorIds[index]);
}

void ScheduleAppointmentDialog::populateTimeSlots(int doctorId)
{
    static const QStringList ALL_SLOTS = {
        "08:00 AM", "09:00 AM", "10:00 AM", "11:00 AM",
        "12:00 PM", "02:00 PM", "03:00 PM", "04:00 PM",
        "05:00 PM", "06:00 PM"
    };

    QString dateStr = m_datePicker->date().toString("yyyy-MM-dd");

    QSet<QString> booked;
    for (const Appointment &a : m_hs->getAppointments()) {
        if (a.getDoctorId() == doctorId
            && a.getStatus() != "Cancelled"
            && QString::fromStdString(a.getDate()) == dateStr)
        {
            booked.insert(QString::fromStdString(a.getTime()));
        }
    }

    m_timeCombo->clear();
    for (const QString &slot : ALL_SLOTS) {
        if (!booked.contains(slot))
            m_timeCombo->addItem(slot);
    }

    if (m_timeCombo->count() == 0)
        m_timeCombo->addItem("No slots available");
}

QString ScheduleAppointmentDialog::date() const
{
    return m_datePicker->date().toString("yyyy-MM-dd");
}

void ScheduleAppointmentDialog::applyStyles()
{
    setStyleSheet(QString(R"(
        QDialog            { background: %1; }
        #dialogHeader      { background: %2; }
        #dialogTitle       { color: #FFFFFF; font-size:15px; font-weight:700; }
        #dialogBody        { background: %1; }
        QFormLayout QLabel { color: %3; font-size:13px; font-weight:600; }
        #formControl, QComboBox#formControl, QDateEdit#formControl {
            background: #FFFFFF;
            border: 1.5px solid %4;
            border-radius: 7px;
            padding: 6px 10px;
            font-size: 13px;
            color: %3;
            min-height: 34px;
        }
        #formControl:focus { border-color: %5; }
        QComboBox#formControl::drop-down { border:none; width:24px; }
        #primaryBtn {
            background: %5; color:#FFF; border:none;
            border-radius:8px; font-size:14px; font-weight:700; padding:0 24px;
        }
        #primaryBtn:hover  { background: %6; }
        #cancelBtn {
            background:transparent; color:%7;
            border:1.5px solid %4; border-radius:8px;
            font-size:14px; font-weight:600; padding:0 20px;
        }
        #cancelBtn:hover   { background: %4; }
    )").arg(APal::BG_PAGE, APal::HEADER_BG, APal::TEXT_PRIMARY,
            APal::BORDER,  APal::ACCENT,    APal::ACCENT_HOVER,
            APal::TEXT_MUTED));
}

AppointmentPage::AppointmentPage(HospitalSystem *hs, QWidget *parent)
    : QWidget(parent), m_hs(hs)
{
    setupUi();
    applyStyles();
    refresh();
    emit dataChanged();
}

void AppointmentPage::setupUi()
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(0);
    root->setContentsMargins(0, 0, 0, 0);

    auto *topBar = new QFrame;
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(72);
    auto *topLay = new QHBoxLayout(topBar);
    topLay->setContentsMargins(28, 0, 28, 0);
    topLay->setSpacing(16);

    auto *titleBlock = new QVBoxLayout;
    titleBlock->setSpacing(2);
    auto *pageTitle = new QLabel("Appointments");
    pageTitle->setObjectName("pageTitle");
    auto *pageSub = new QLabel("Schedule and manage patient appointments");
    pageSub->setObjectName("pageSub");
    titleBlock->addWidget(pageTitle);
    titleBlock->addWidget(pageSub);

    m_filterCombo = new QComboBox;
    m_filterCombo->setObjectName("filterCombo");
    m_filterCombo->addItems({"All", "Scheduled", "Completed", "Cancelled"});
    m_filterCombo->setFixedWidth(150);
    m_filterCombo->setFixedHeight(38);

    m_scheduleBtn = new QPushButton("＋  Schedule");
    m_scheduleBtn->setObjectName("genBtn");
    m_scheduleBtn->setFixedHeight(40);

    topLay->addLayout(titleBlock);
    topLay->addStretch();
    topLay->addWidget(m_filterCombo);
    topLay->addWidget(m_scheduleBtn);
    root->addWidget(topBar);

    auto *strip = new QFrame;
    strip->setObjectName("summaryStrip");
    strip->setFixedHeight(36);
    auto *sLay = new QHBoxLayout(strip);
    sLay->setContentsMargins(28, 0, 28, 0);
    m_summaryLabel = new QLabel;
    m_summaryLabel->setObjectName("summaryLabel");
    sLay->addWidget(m_summaryLabel);
    sLay->addStretch();
    root->addWidget(strip);

    m_table = new QTableWidget(0, COL_COUNT);
    m_table->setObjectName("apptTable");
    m_table->setHorizontalHeaderLabels({
        "ID", "Patient", "Doctor", "Date", "Time", "Status", "Action"
    });
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_table->setColumnWidth(COL_ID,      60);
    m_table->setColumnWidth(COL_PATIENT, 160);
    m_table->setColumnWidth(COL_DOCTOR,  180);
    m_table->setColumnWidth(COL_DATE,    110);
    m_table->setColumnWidth(COL_TIME,    100);
    m_table->setColumnWidth(COL_STATUS,  110);
    m_table->horizontalHeader()->setSectionResizeMode(COL_ACTION, QHeaderView::Stretch);

    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(false);
    m_table->setShowGrid(false);
    m_table->verticalHeader()->setDefaultSectionSize(52);
    m_table->horizontalHeader()->setStretchLastSection(true);
    root->addWidget(m_table, 1);

    connect(m_scheduleBtn, &QPushButton::clicked,
            this, &AppointmentPage::openScheduleDialog);
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AppointmentPage::applyFilter);
}

void AppointmentPage::refresh()
{
    const QString filter = m_filterCombo->currentText();
    const auto &appts    = m_hs->getAppointments();

    QVector<const Appointment *> visible;
    for (const Appointment &a : appts) {
        const QString status = QString::fromStdString(a.getStatus());
        if (filter == "All" || filter == status)
            visible.append(&a);
    }

    m_table->setRowCount(0);
    m_table->setRowCount(visible.size());
    for (int row = 0; row < visible.size(); ++row)
        populateRow(row, *visible[row]);

    int scheduled = 0, completed = 0, cancelled = 0;
    for (const Appointment &a : appts) {
        const std::string &s = a.getStatus();
        if (s == "Scheduled")  ++scheduled;
        else if (s == "Completed") ++completed;
        else if (s == "Cancelled") ++cancelled;
    }
    m_summaryLabel->setText(
        QString("Total: <b>%1</b>  |  "
                "<span style='color:%2'>Scheduled: <b>%3</b></span>  |  "
                "<span style='color:%4'>Completed: <b>%5</b></span>  |  "
                "<span style='color:%6'>Cancelled: <b>%7</b></span>")
        .arg(appts.size())
        .arg(APal::WARNING).arg(scheduled)
        .arg(APal::SUCCESS).arg(completed)
        .arg(APal::DANGER).arg(cancelled));
}

void AppointmentPage::populateRow(int row, const Appointment &a)
{
    auto cell = [&](int col, const QString &text,
                    Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter)
    {
        auto *item = new QTableWidgetItem(text);
        item->setTextAlignment(align);
        m_table->setItem(row, col, item);
        return item;
    };

    QString patName = "(unknown)";
    if (Patient *p = m_hs->findPatient(a.getPatientId()))
        patName = QString::fromStdString(p->getName());

    QString docName = "(unknown)";
    if (Doctor *d = m_hs->findDoctor(a.getDoctorId()))
        docName = "Dr. " + QString::fromStdString(d->getName());

    const QString status = QString::fromStdString(a.getStatus());

    cell(COL_ID,     QString("#%1").arg(a.getAppointmentId()),
                     Qt::AlignCenter | Qt::AlignVCenter);
    cell(COL_PATIENT, patName);
    cell(COL_DOCTOR,  docName);
    cell(COL_DATE,    QString::fromStdString(a.getDate()),
                      Qt::AlignCenter | Qt::AlignVCenter);
    cell(COL_TIME,    QString::fromStdString(a.getTime()),
                      Qt::AlignCenter | Qt::AlignVCenter);

    auto *statusItem = cell(COL_STATUS, status, Qt::AlignCenter | Qt::AlignVCenter);
    QColor rowBg;
    if (status == "Scheduled")  {
        statusItem->setForeground(QBrush(QColor(APal::WARNING)));
        rowBg = QColor(APal::WARNING_BG);
    } else if (status == "Completed") {
        statusItem->setForeground(QBrush(QColor(APal::SUCCESS)));
        rowBg = QColor(APal::SUCCESS_BG);
    } else {
        statusItem->setForeground(QBrush(QColor(APal::DANGER)));
        rowBg = QColor(APal::DANGER_BG);
    }
    for (int c = 0; c < COL_ACTION; ++c)
        if (auto *it = m_table->item(row, c))
            it->setBackground(rowBg);

    auto *cellWidget = new QWidget;
    cellWidget->setStyleSheet(
        QString("background: %1;").arg(rowBg.name()));
    auto *lay = new QHBoxLayout(cellWidget);
    lay->setContentsMargins(8, 6, 8, 6);
    lay->setSpacing(8);

    const int id = a.getAppointmentId();

    if (status == "Scheduled") {

        auto *completeBtn = new QPushButton("Complete");
        completeBtn->setObjectName("completeBtn");
        completeBtn->setFixedHeight(30);
        connect(completeBtn, &QPushButton::clicked,
                this, [this, id]{ onCompleteClicked(id); });
        lay->addWidget(completeBtn);

        auto *cancelBtn = new QPushButton("Cancel");
        cancelBtn->setObjectName("cancelApptBtn");
        cancelBtn->setFixedHeight(30);
        connect(cancelBtn, &QPushButton::clicked,
                this, [this, id]{ onCancelClicked(id); });
        lay->addWidget(cancelBtn);
    }

    lay->addStretch();
    m_table->setCellWidget(row, COL_ACTION, cellWidget);
}

void AppointmentPage::onCompleteClicked(int appointmentId)
{

    m_hs->completeAppointment(appointmentId);
    refresh();
    emit dataChanged();  
}

void AppointmentPage::onCancelClicked(int appointmentId)
{
    auto reply = QMessageBox::question(
        this, "Cancel Appointment",
        QString("Cancel Appointment #%1?").arg(appointmentId),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_hs->cancelAppointment(appointmentId);
        refresh();
        emit dataChanged();  
    }
}

void AppointmentPage::applyFilter(int)
{
    refresh();
    emit dataChanged();
}

void AppointmentPage::openScheduleDialog()
{
    ScheduleAppointmentDialog dlg(m_hs, this);
    if (dlg.exec() != QDialog::Accepted) return;

    if (dlg.patientId() < 0 || dlg.doctorId() < 0) return;

    if (dlg.time() == "No slots available") {
        QMessageBox::warning(this, "No Slots",
            "There are no available time slots for the selected doctor on that date.");
        return;
    }

    m_hs->scheduleAppointment(
        dlg.patientId(),
        dlg.doctorId(),
        dlg.date().toStdString(),
        dlg.time().toStdString());

    refresh();
    emit dataChanged();
}

void AppointmentPage::applyStyles()
{
    setStyleSheet(QString(R"(
        AppointmentPage { background: %1; }

        #topBar {
            background: %2;
            border-bottom: none;
        }
        #pageTitle  { font-size:20px; font-weight:800; color:%4; background: transparent; }
        #pageSub    { font-size:12px; color:%5; background: transparent; }

        #filterCombo {
            background:#FFF; border:1.5px solid %3;
            border-radius:8px; padding:4px 12px;
            font-size:13px; font-weight:600; color:%4;
        }
        #filterCombo::drop-down { border:none; width:24px; }
        #filterCombo:hover { border-color:%6; }

        #genBtn {
            background:%6; color:#FFF; border:none;
            border-radius:9px; font-size:14px; font-weight:700; padding:0 22px;
        }
        #genBtn:hover { background:%7; }

        #summaryStrip { background:%2; border-bottom:1px solid %3; }
        #summaryLabel { font-size:12px; color:%4; background: transparent; }

        #apptTable {
            background:#FFF; border:none;
            gridline-color:transparent; font-size:13px; color:%4;
        }
        #apptTable QHeaderView::section {
            background:%8; color:%9; font-size:11px; font-weight:700;
            letter-spacing:1px; text-transform:uppercase;
            padding:10px 12px; border:none;
            border-right:1px solid rgba(255,255,255,0.1);
        }
        #apptTable::item { padding:0 12px; border-bottom:1px solid %3; }
        #apptTable::item:selected { background:%10; color:%4; }

        #completeBtn {
            background:%11; color:%12; border:1.5px solid %12;
            border-radius:6px; font-size:12px; font-weight:700; padding:0 10px;
        }
        #completeBtn:hover { background:%12; color:#FFF; }

        #cancelApptBtn {
            background:%13; color:%14; border:1.5px solid %14;
            border-radius:6px; font-size:12px; font-weight:700; padding:0 10px;
        }
        #cancelApptBtn:hover { background:%14; color:#FFF; }
    )").arg(
        APal::BG_PAGE,      
        APal::BG_CARD,      
        APal::BORDER,       
        APal::TEXT_PRIMARY, 
        APal::TEXT_MUTED,   
        APal::ACCENT,       
        APal::ACCENT_HOVER, 
        APal::HEADER_BG,    
        APal::HEADER_FG,    
        APal::ACCENT_LIGHT, 
        APal::SUCCESS_BG,   
        APal::SUCCESS,      
        APal::DANGER_BG,    
        APal::DANGER        
    ));
}
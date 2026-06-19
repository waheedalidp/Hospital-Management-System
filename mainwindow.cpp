#include "mainwindow.h"
#include "dashboardwidget.h"
#include <algorithm>
#include <QApplication>
#include <QFont>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QFrame>
#include <QDateTime>

static const QStringList PAGE_COLORS = {
    "#1ABC9C",   
    "#3498DB",   
    "#9B59B6",   
    "#E67E22",   
    "#E74C3C"    
};

static const QStringList PAGE_ICONS = {
    "⊞",
    "♥",
    "✚",
    "◷",
    "₿"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Hospital Management System");
    setMinimumSize(1100, 700);
    resize(1280, 780);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    bodyLayout = new QHBoxLayout();
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);

    setupTopBar();
    setupSidebar();
    setupStackedWidget();
    setupStyles();

    rootLayout->addWidget(topBar);
    rootLayout->addLayout(bodyLayout);

    onSidebarButtonClicked(0);
}

MainWindow::~MainWindow() {}

void MainWindow::setupTopBar()
{
    topBar = new QWidget();
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(60);

    topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(24, 0, 24, 0);

    titleLabel = new QLabel("🏥  Hospital Management System");
    titleLabel->setObjectName("titleLabel");

    QLabel *dateLabel = new QLabel(QDateTime::currentDateTime().toString("dddd, MMMM d yyyy"));
    dateLabel->setObjectName("dateLabel");

    topBarLayout->addWidget(titleLabel);
    topBarLayout->addStretch();
    topBarLayout->addWidget(dateLabel);
}

void MainWindow::setupSidebar()
{
    sidebar = new QWidget();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(230);

    sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 20);
    sidebarLayout->setSpacing(4);

    logoLabel = new QLabel("  MedCore");
    logoLabel->setObjectName("logoLabel");
    logoLabel->setFixedHeight(70);
    logoLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    sidebarLayout->addWidget(logoLabel);

    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setObjectName("sidebarDivider");
    sidebarLayout->addWidget(divider);
    sidebarLayout->addSpacing(8);

    QStringList labels = { "Dashboard", "Patients", "Doctors", "Appointments", "Billing" };

    for (int i = 0; i < labels.size(); ++i) {
        QPushButton *btn = createSidebarButton(labels[i], PAGE_ICONS[i]);
        btn->setProperty("navIndex", i);
        connect(btn, &QPushButton::clicked, this, [this, i]() {
            onSidebarButtonClicked(i);
        });
        sidebarButtons.append(btn);
        sidebarLayout->addWidget(btn);
    }

    sidebarLayout->addStretch();

    QLabel *versionLabel = new QLabel("  v1.0.0");
    versionLabel->setObjectName("versionLabel");
    sidebarLayout->addWidget(versionLabel);

    bodyLayout->addWidget(sidebar);
}

QPushButton *MainWindow::createSidebarButton(const QString &text, const QString &iconSymbol)
{
    QPushButton *btn = new QPushButton(QString("  %1   %2").arg(iconSymbol, text));
    btn->setObjectName("sidebarBtn");
    btn->setFixedHeight(48);
    btn->setCheckable(false);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFlat(true);
    return btn;
}

QVector<AppointmentRecord> MainWindow::buildAppointmentRecords()
{
    QVector<AppointmentRecord> apptRecords;
    for (const auto &a : hs_.getAppointments()) {
        AppointmentRecord r;
        r.id     = QString("A-%1").arg(a.getAppointmentId(), 3, 10, QChar('0'));
        r.time   = QString::fromStdString(a.getTime());
        r.status = QString::fromStdString(a.getStatus());

        Patient *pat = hs_.findPatient(a.getPatientId());
        Doctor  *doc = hs_.findDoctor(a.getDoctorId());

        r.patient = pat
            ? QString::fromStdString(pat->getName())
            : QString("Unknown (ID %1)").arg(a.getPatientId());

        r.doctor  = doc
            ? "Dr. " + QString::fromStdString(doc->getName())
            : QString("Unknown (ID %1)").arg(a.getDoctorId());

        apptRecords.append(r);
    }
    return apptRecords;
}

void MainWindow::setupStackedWidget()
{
    stackedWidget = new QStackedWidget();
    stackedWidget->setObjectName("contentArea");

    dashboardWidget_ = new DashboardWidget(this);

    int unpaid = (int)std::count_if(hs_.getBills().begin(), hs_.getBills().end(),
                                    [](const Bill &b){ return !b.getIsPaid(); });
    int scheduledCount = (int)std::count_if(hs_.getAppointments().begin(), hs_.getAppointments().end(),
                                    [](const Appointment &a){ return a.getStatus() == "Scheduled"; });
    dashboardWidget_->updateStats(
        (int)hs_.getPatients().size(),
        (int)hs_.getDoctors().size(),
        scheduledCount,
        unpaid
    );

    dashboardWidget_->setAppointments(buildAppointmentRecords());
    stackedWidget->addWidget(dashboardWidget_);

    patientPage_ = new PatientPage(&hs_, this);
    stackedWidget->addWidget(patientPage_);
    connect(patientPage_, &PatientPage::dataChanged, this, &MainWindow::refreshDashboard);

    doctorPage_ = new DoctorPage(&hs_, this);
    stackedWidget->addWidget(doctorPage_);
    connect(doctorPage_, &DoctorPage::dataChanged, this, &MainWindow::refreshDashboard);

    appointmentPage_ = new AppointmentPage(&hs_, this);
    stackedWidget->addWidget(appointmentPage_);
    connect(appointmentPage_, &AppointmentPage::dataChanged, this, &MainWindow::refreshDashboard);

    billingPage_ = new BillingPage(&hs_, this);
    stackedWidget->addWidget(billingPage_);

    bodyLayout->addWidget(stackedWidget, 1);
}

QWidget *MainWindow::createPage(const QString &pageName, const QString &accentColor)
{
    QWidget *page = new QWidget();
    page->setObjectName("page");

    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *badge = new QLabel(pageName);
    badge->setObjectName("pageBadge");
    badge->setAlignment(Qt::AlignCenter);
    badge->setFixedSize(240, 240);
    badge->setStyleSheet(
        QString("QLabel {"
                "  background-color: %1;"
                "  color: white;"
                "  border-radius: 120px;"
                "  font-size: 28px;"
                "  font-weight: 700;"
                "  letter-spacing: 1px;"
                "}").arg(accentColor)
    );

    QLabel *subLabel = new QLabel(
        QString("The %1 module is under construction.\nContent will appear here.").arg(pageName));
    subLabel->setObjectName("pageSubLabel");
    subLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(badge, 0, Qt::AlignCenter);
    layout->addSpacing(24);
    layout->addWidget(subLabel, 0, Qt::AlignCenter);
    layout->addStretch();

    return page;
}

void MainWindow::onSidebarButtonClicked(int index)
{
    if (index < 0 || index >= sidebarButtons.size()) return;
    currentIndex = index;
    stackedWidget->setCurrentIndex(index);
    updateButtonStyles(index);
}

void MainWindow::updateButtonStyles(int activeIndex)
{
    for (int i = 0; i < sidebarButtons.size(); ++i) {
        sidebarButtons[i]->setProperty("active", i == activeIndex);
        sidebarButtons[i]->style()->unpolish(sidebarButtons[i]);
        sidebarButtons[i]->style()->polish(sidebarButtons[i]);
    }
}

void MainWindow::setupStyles()
{
    qApp->setStyleSheet(R"(

QMainWindow, QWidget {
    background-color: #F0F4F8;
    color: #2D3748;
    font-family: "Segoe UI", "San Francisco", "Helvetica Neue", Arial, sans-serif;
    font-size: 14px;
}

QWidget#topBar {
    background-color: #FFFFFF;
    border-bottom: 1px solid #E2E8F0;
}
QLabel#titleLabel {
    font-size: 18px;
    font-weight: 700;
    color: #1A202C;
    letter-spacing: 0.3px;
    background-color: #FFFFFF;
}
QLabel#dateLabel {
    font-size: 13px;
    color: #718096;
    background-color: #FFFFFF;
}

QWidget#sidebar {
    background-color: #2C3E50;
    border-right: 1px solid #1a252f;
}
QLabel#logoLabel {
    font-size: 24px;
    font-weight: 900;
    color: #FFFFFF;
    letter-spacing: 2px;
    padding-left: 20px;
    background-color: #1ABC9C;
}
QFrame#sidebarDivider {
    color: #3d5166;
    margin: 0 16px;
}
QPushButton#sidebarBtn {
    background-color: transparent;
    color: #94A3B8;
    border: none;
    border-radius: 0px;
    text-align: left;
    padding-left: 20px;
    font-size: 14px;
    font-weight: 500;
    letter-spacing: 0.2px;
}
QPushButton#sidebarBtn:hover {
    background-color: #34495E;
    color: #E2E8F0;
}
QPushButton#sidebarBtn[active="true"] {
    background-color: #1ABC9C;
    color: #FFFFFF;
    font-weight: 700;
    border-left: 4px solid #FFFFFF;
    padding-left: 16px;
}
QPushButton#sidebarBtn[active="true"]:hover {
    background-color: #16A085;
}
QLabel#versionLabel {
    color: #4A6278;
    font-size: 12px;
    padding-left: 20px;
}

QStackedWidget#contentArea {
    background-color: #F0F4F8;
}
QWidget#page {
    background-color: #F0F4F8;
}
QLabel#pageSubLabel {
    color: #718096;
    font-size: 15px;
    line-height: 1.6;
}

QScrollBar:vertical {
    background: #F0F4F8;
    width: 8px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background: #CBD5E0;
    border-radius: 4px;
    min-height: 20px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0;
}

    )");
}

void MainWindow::refreshDashboard()
{
    if (!dashboardWidget_) return;

    int unpaid = (int)std::count_if(hs_.getBills().begin(), hs_.getBills().end(),
                                    [](const Bill &b){ return !b.getIsPaid(); });

    int scheduledCount = (int)std::count_if(
        hs_.getAppointments().begin(), hs_.getAppointments().end(),
        [](const Appointment &a){ return a.getStatus() == "Scheduled"; });

    dashboardWidget_->updateStats(
        (int)hs_.getPatients().size(),
        (int)hs_.getDoctors().size(),
        scheduledCount,
        unpaid
    );

    dashboardWidget_->setAppointments(buildAppointmentRecords());
}
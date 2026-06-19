#include "BillingPage.h"

#include <QApplication>
#include <QScreen>

namespace Palette {
    const QString BG_PAGE       = "#F0F4F8";
    const QString BG_CARD       = "#FFFFFF";
    const QString ACCENT        = "#1A6FC4";
    const QString ACCENT_HOVER  = "#155AA0";
    const QString ACCENT_LIGHT  = "#E8F1FB";
    const QString SUCCESS       = "#1B7A48";
    const QString SUCCESS_BG    = "#E6F4EC";
    const QString DANGER        = "#C0392B";
    const QString DANGER_BG     = "#FDECEA";
    const QString TEXT_PRIMARY  = "#1A202C";
    const QString TEXT_MUTED    = "#718096";
    const QString BORDER        = "#CBD5E0";
    const QString ROW_PAID      = "#E6F4EC";
    const QString ROW_UNPAID    = "#FDECEA";
    const QString HEADER_BG     = "#1A3557";
    const QString HEADER_FG     = "#FFFFFF";
}

GenerateBillDialog::GenerateBillDialog(HospitalSystem *hs, QWidget *parent)
    : QDialog(parent), m_hs(hs)
{
    setWindowTitle("Generate New Bill");
    setModal(true);
    setMinimumWidth(440);
    setupUi();
    applyStyles();
    recalcTotal();
}

void GenerateBillDialog::setupUi()
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(0);
    root->setContentsMargins(0, 0, 0, 0);

    auto *header = new QFrame;
    header->setObjectName("dialogHeader");
    header->setFixedHeight(64);
    header->setAutoFillBackground(true);
    auto *hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(24, 0, 24, 0);

    auto *icon = new QLabel("🧾");
    icon->setStyleSheet("font-size: 24px; background:transparent; color:#FFFFFF;");

    auto *title = new QLabel("Generate New Bill");
    title->setObjectName("dialogTitle");
    title->setStyleSheet("color:#FFFFFF; font-size:15px; font-weight:700; background:transparent;");

    hLayout->addWidget(icon);
    hLayout->addSpacing(10);
    hLayout->addWidget(title);
    hLayout->addStretch();
    root->addWidget(header);

    auto *body = new QWidget;
    body->setObjectName("dialogBody");
    auto *form = new QFormLayout(body);
    form->setSpacing(14);
    form->setContentsMargins(28, 24, 28, 8);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_patientCombo = new QComboBox;
    m_patientCombo->setObjectName("formControl");
    for (const Patient &p : m_hs->getPatients()) {
        m_patientCombo->addItem(
            QString("%1 (ID %2)")
                .arg(QString::fromStdString(p.getName()))
                .arg(p.getId()));
        m_patientIds.append(p.getId());
    }
    form->addRow("Patient :", m_patientCombo);

    m_datePicker = new QDateEdit(QDate::currentDate());
    m_datePicker->setObjectName("formControl");
    m_datePicker->setCalendarPopup(true);
    m_datePicker->setDisplayFormat("dd MMM yyyy");
    form->addRow("Date :", m_datePicker);

    auto *sep1 = new QFrame;
    sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet(QString("color: %1; margin: 4px 0;").arg(Palette::BORDER));
    form->addRow(sep1);

    auto *feeLbl = new QLabel("Fee Breakdown");
    feeLbl->setStyleSheet(QString("color: %1; font-weight: 700; font-size: 12px; "
                                  "text-transform: uppercase; letter-spacing: 1px;")
                                  .arg(Palette::TEXT_MUTED));
    form->addRow(feeLbl);

    m_consultSpin = new QDoubleSpinBox;
    m_consultSpin->setObjectName("formControl");
    m_consultSpin->setRange(0, 999999);
    m_consultSpin->setDecimals(2);
    m_consultSpin->setPrefix("PKR  ");
    m_consultSpin->setSingleStep(100);
    m_consultSpin->setValue(1500);
    form->addRow("Consultation :", m_consultSpin);

    m_medicineSpin = new QDoubleSpinBox;
    m_medicineSpin->setObjectName("formControl");
    m_medicineSpin->setRange(0, 999999);
    m_medicineSpin->setDecimals(2);
    m_medicineSpin->setPrefix("PKR  ");
    m_medicineSpin->setSingleStep(100);
    m_medicineSpin->setValue(0);
    form->addRow("Medicine :", m_medicineSpin);

    m_testSpin = new QDoubleSpinBox;
    m_testSpin->setObjectName("formControl");
    m_testSpin->setRange(0, 999999);
    m_testSpin->setDecimals(2);
    m_testSpin->setPrefix("PKR  ");
    m_testSpin->setSingleStep(100);
    m_testSpin->setValue(0);
    form->addRow("Tests :", m_testSpin);

    root->addWidget(body);

    auto *totalBar = new QFrame;
    totalBar->setObjectName("totalBar");
    auto *tLayout = new QHBoxLayout(totalBar);
    tLayout->setContentsMargins(28, 14, 28, 14);

    auto *totalTag = new QLabel("TOTAL AMOUNT");
    totalTag->setObjectName("totalTag");

    m_totalLabel = new QLabel("PKR  0.00");
    m_totalLabel->setObjectName("totalValue");

    tLayout->addWidget(totalTag);
    tLayout->addStretch();
    tLayout->addWidget(m_totalLabel);
    root->addWidget(totalBar);

    auto *btnBar = new QWidget;
    btnBar->setObjectName("dialogBody");
    auto *btnLayout = new QHBoxLayout(btnBar);
    btnLayout->setContentsMargins(28, 12, 28, 20);
    btnLayout->setSpacing(12);

    m_cancelBtn = new QPushButton("Cancel");
    m_cancelBtn->setObjectName("cancelBtn");
    m_cancelBtn->setFixedHeight(40);

    m_generateBtn = new QPushButton("  Generate Bill");
    m_generateBtn->setObjectName("primaryBtn");
    m_generateBtn->setFixedHeight(40);

    btnLayout->addStretch();
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_generateBtn);
    root->addWidget(btnBar);

    connect(m_consultSpin,  QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &GenerateBillDialog::recalcTotal);
    connect(m_medicineSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &GenerateBillDialog::recalcTotal);
    connect(m_testSpin,     QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &GenerateBillDialog::recalcTotal);

    connect(m_generateBtn, &QPushButton::clicked, this, [this]() {
        if (m_patientIds.isEmpty()) {
            QMessageBox::warning(this, "No Patients",
                "No patients are registered yet. Please add a patient first.");
            return;
        }

        if (m_consultSpin->value() < 0 ||
            m_medicineSpin->value() < 0 ||
            m_testSpin->value() < 0) {
            QMessageBox::warning(this, "Invalid Fees",
                "Fee values cannot be negative.");
            return;
        }

        double total = m_consultSpin->value()
                     + m_medicineSpin->value()
                     + m_testSpin->value();
        if (total <= 0.0) {
            QMessageBox::warning(this, "Invalid Fees",
                "At least one fee must be greater than zero.");
            return;
        }
        accept();
    });
    connect(m_cancelBtn,   &QPushButton::clicked, this, &QDialog::reject);
}

void GenerateBillDialog::recalcTotal()
{
    double total = m_consultSpin->value()
                 + m_medicineSpin->value()
                 + m_testSpin->value();
    m_totalLabel->setText(QString("PKR  %L1").arg(total, 0, 'f', 2));
}

BillRecord GenerateBillDialog::result() const
{
    BillRecord r;

    int idx = m_patientCombo->currentIndex();
    if (idx >= 0 && idx < m_patientIds.size()) {
        if (Patient *p = m_hs->findPatient(m_patientIds[idx]))
            r.patientName = QString::fromStdString(p->getName());
        else
            r.patientName = m_patientCombo->currentText();
        const_cast<GenerateBillDialog*>(this)->m_selectedPatientId = m_patientIds[idx];
    } else {
        r.patientName = m_patientCombo->currentText();
    }
    r.consultationFee = m_consultSpin->value();
    r.medicineFee     = m_medicineSpin->value();
    r.testFee         = m_testSpin->value();
    r.total           = r.consultationFee + r.medicineFee + r.testFee;
    r.paid            = false;
    r.date            = m_datePicker->date();
    return r;
}

void GenerateBillDialog::applyStyles()
{
    setStyleSheet(QString(R"(
        QDialog {
            background: %1;
        }
        #dialogHeader {
            background: %2;
            border-radius: 0px;
        }
        #dialogTitle {
            color: #FFFFFF;
            font-size: 16px;
            font-weight: 700;
        }
        #dialogBody {
            background: %1;
        }
        QFormLayout QLabel {
            color: %3;
            font-size: 13px;
            font-weight: 600;
        }
        #formControl, QComboBox#formControl, QDoubleSpinBox#formControl,
        QDateEdit#formControl {
            background: #FFFFFF;
            border: 1.5px solid %4;
            border-radius: 7px;
            padding: 6px 10px;
            font-size: 13px;
            color: %5;
            min-height: 34px;
        }
        #formControl:focus, QComboBox#formControl:focus,
        QDoubleSpinBox#formControl:focus, QDateEdit#formControl:focus {
            border-color: %6;
        }
        QComboBox#formControl::drop-down {
            border: none;
            width: 24px;
        }
        #totalBar {
            background: %7;
            border-top: 2px solid %6;
        }
        #totalTag {
            color: %3;
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 1.5px;
        }
        #totalValue {
            color: %2;
            font-size: 20px;
            font-weight: 800;
        }
        #primaryBtn {
            background: %6;
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 700;
            padding: 0 24px;
        }
        #primaryBtn:hover {
            background: %8;
        }
        #cancelBtn {
            background: transparent;
            color: %3;
            border: 1.5px solid %4;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
            padding: 0 20px;
        }
        #cancelBtn:hover {
            background: %4;
        }
    )").arg(
        Palette::BG_PAGE,        
        Palette::HEADER_BG,      
        Palette::TEXT_MUTED,     
        Palette::BORDER,         
        Palette::TEXT_PRIMARY,   
        Palette::ACCENT,         
        Palette::ACCENT_LIGHT,   
        Palette::ACCENT_HOVER    
    ));
}

ReceiptDialog::ReceiptDialog(const BillRecord &bill, QWidget *parent)
    : QDialog(parent), m_bill(bill)
{
    setWindowTitle(QString("Receipt — Bill #%1").arg(bill.billId));
    setModal(true);
    setMinimumSize(440, 560);
    setupUi();
    applyStyles();
}

void ReceiptDialog::setupUi()
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(0);
    root->setContentsMargins(0, 0, 0, 0);

    auto *header = new QFrame;
    header->setObjectName("rcptHeader");
    header->setFixedHeight(64);
    auto *hLay = new QHBoxLayout(header);
    hLay->setContentsMargins(24, 0, 24, 0);
    auto *hIcon = new QLabel("🏥");
    hIcon->setStyleSheet("font-size: 24px;");
    auto *hTitle = new QLabel("Payment Receipt");
    hTitle->setObjectName("rcptHeaderTitle");
    hLay->addWidget(hIcon);
    hLay->addSpacing(10);
    hLay->addWidget(hTitle);
    hLay->addStretch();
    root->addWidget(header);

    m_receiptView = new QTextEdit;
    m_receiptView->setObjectName("receiptView");
    m_receiptView->setReadOnly(true);
    m_receiptView->setHtml(buildReceiptText());
    root->addWidget(m_receiptView, 1);

    auto *btnBar = new QFrame;
    btnBar->setObjectName("rcptBtnBar");
    auto *btnLay = new QHBoxLayout(btnBar);
    btnLay->setContentsMargins(24, 12, 24, 16);
    btnLay->setSpacing(12);

    m_closeBtn = new QPushButton("Close");
    m_closeBtn->setObjectName("rcptCloseBtn");
    m_closeBtn->setFixedHeight(40);

    m_saveBtn = new QPushButton("🖨  Print / Save as .txt");
    m_saveBtn->setObjectName("rcptSaveBtn");
    m_saveBtn->setFixedHeight(40);

    btnLay->addStretch();
    btnLay->addWidget(m_closeBtn);
    btnLay->addWidget(m_saveBtn);
    root->addWidget(btnBar);

    connect(m_saveBtn,  &QPushButton::clicked, this, &ReceiptDialog::saveReceipt);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

QString ReceiptDialog::buildReceiptText() const
{
    const QString statusColor = m_bill.paid
        ? QString("#1B7A48") : QString("#C0392B");
    const QString statusText  = m_bill.paid ? "✔  PAID" : "✘  UNPAID";
    const QString statusBg    = m_bill.paid ? "#E6F4EC" : "#FDECEA";

    return QString(R"(
<html><body style="font-family: 'Courier New', monospace; margin: 20px;">

<div style="text-align:center; padding:16px 0 8px;">
  <p style="font-size:22px; font-weight:900; color:#1A3557; margin:0;">
    HealthCore HMS
  </p>
  <p style="font-size:11px; color:#718096; margin:4px 0 0;">
    123 Medical Plaza, Karachi — Tel: +92-21-0000000
  </p>
  <hr style="border:1px dashed #CBD5E0; margin:14px 0;">
</div>

<table width="100%" cellspacing="0" cellpadding="4"
       style="font-size:13px; color:#1A202C;">
  <tr>
    <td><b>Bill ID</b></td>
    <td align="right">#%1</td>
  </tr>
  <tr>
    <td><b>Patient</b></td>
    <td align="right">%2</td>
  </tr>
  <tr>
    <td><b>Date</b></td>
    <td align="right">%3</td>
  </tr>
</table>

<hr style="border:1px dashed #CBD5E0; margin:14px 0;">

<p style="font-size:11px; font-weight:700; color:#718096;
          letter-spacing:1.5px; margin:0 0 8px;">
  FEE BREAKDOWN
</p>

<table width="100%" cellspacing="0" cellpadding="5"
       style="font-size:13px; color:#1A202C;">
  <tr>
    <td>Consultation Fee</td>
    <td align="right">PKR %4</td>
  </tr>
  <tr>
    <td>Medicine Fee</td>
    <td align="right">PKR %5</td>
  </tr>
  <tr>
    <td>Test Fee</td>
    <td align="right">PKR %6</td>
  </tr>
</table>

<hr style="border:1px solid #1A3557; margin:12px 0 8px;">

<table width="100%" cellspacing="0" cellpadding="4">
  <tr>
    <td style="font-size:15px; font-weight:900; color:#1A3557;">TOTAL</td>
    <td align="right" style="font-size:18px; font-weight:900; color:#1A3557;">
      PKR %7
    </td>
  </tr>
</table>

<div style="text-align:center; margin:18px 0 4px;">
  <span style="background:%8; color:%9; font-weight:800;
               font-size:13px; padding:6px 24px; border-radius:20px;">
    %10
  </span>
</div>

<hr style="border:1px dashed #CBD5E0; margin:16px 0 8px;">
<p style="text-align:center; font-size:11px; color:#718096;">
  Thank you for choosing HealthCore HMS.<br>
  This is a computer-generated receipt.
</p>
</body></html>
    )")
    .arg(m_bill.billId)
    .arg(m_bill.patientName)
    .arg(m_bill.date.toString("dd MMM yyyy"))
    .arg(m_bill.consultationFee, 0, 'f', 2)
    .arg(m_bill.medicineFee,     0, 'f', 2)
    .arg(m_bill.testFee,         0, 'f', 2)
    .arg(m_bill.total,           0, 'f', 2)
    .arg(statusBg)
    .arg(statusColor)
    .arg(statusText);
}

void ReceiptDialog::saveReceipt()
{
    const QString defaultName =
        QString("receipt_%1_%2.txt")
        .arg(m_bill.billId)
        .arg(m_bill.date.toString("yyyyMMdd"));

    const QString path = QFileDialog::getSaveFileName(
        this,
        "Save Receipt",
        QDir::homePath() + "/" + defaultName,
        "Text Files (*.txt);;All Files (*)");

    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error",
            "Could not open file for writing:\n" + path);
        return;
    }

    QTextStream out(&file);

    out << "============================================\n";
    out << "           HEALTHCORE HMS\n";
    out << "   123 Medical Plaza, Karachi\n";
    out << "   Tel: +92-21-0000000\n";
    out << "============================================\n\n";
    out << QString("Bill ID     : #%1\n").arg(m_bill.billId);
    out << QString("Patient     : %1\n").arg(m_bill.patientName);
    out << QString("Date        : %1\n").arg(m_bill.date.toString("dd MMM yyyy"));
    out << "\n--------------------------------------------\n";
    out << "FEE BREAKDOWN\n";
    out << "--------------------------------------------\n";
    out << QString("Consultation: PKR %1\n").arg(m_bill.consultationFee, 10, 'f', 2);
    out << QString("Medicine    : PKR %1\n").arg(m_bill.medicineFee,     10, 'f', 2);
    out << QString("Tests       : PKR %1\n").arg(m_bill.testFee,         10, 'f', 2);
    out << "============================================\n";
    out << QString("TOTAL       : PKR %1\n").arg(m_bill.total, 10, 'f', 2);
    out << "============================================\n";
    out << QString("Status      : %1\n").arg(m_bill.paid ? "PAID" : "UNPAID");
    out << "\nThank you for choosing HealthCore HMS.\n";
    out << "This is a computer-generated receipt.\n";

    file.close();

    QMessageBox::information(this, "Saved",
        "Receipt saved successfully to:\n" + path);
}

void ReceiptDialog::applyStyles()
{
    setStyleSheet(QString(R"(
        QDialog {
            background: %1;
        }
        #rcptHeader {
            background: %2;
        }
        #rcptHeaderTitle {
            color: #FFFFFF;
            font-size: 16px;
            font-weight: 700;
        }
        #receiptView {
            background: #FFFFFF;
            border: none;
            font-family: 'Courier New', monospace;
        }
        #rcptBtnBar {
            background: %1;
            border-top: 1px solid %3;
        }
        #rcptSaveBtn {
            background: %4;
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            font-size: 13px;
            font-weight: 700;
            padding: 0 20px;
        }
        #rcptSaveBtn:hover { background: %5; }
        #rcptCloseBtn {
            background: transparent;
            color: %6;
            border: 1.5px solid %3;
            border-radius: 8px;
            font-size: 13px;
            font-weight: 600;
            padding: 0 18px;
        }
        #rcptCloseBtn:hover { background: %3; }
    )").arg(
        Palette::BG_PAGE,
        Palette::HEADER_BG,
        Palette::BORDER,
        Palette::ACCENT,
        Palette::ACCENT_HOVER,
        Palette::TEXT_MUTED
    ));
}

BillingPage::BillingPage(HospitalSystem *hs, QWidget *parent)
    : QWidget(parent), m_hs(hs)
{
    setupUi();
    applyStyles();
    refresh();   
}

void BillingPage::setupUi()
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
    auto *pageTitle = new QLabel("Billing");
    pageTitle->setObjectName("pageTitle");
    auto *pageSub   = new QLabel("Manage patient bills and invoices");
    pageSub->setObjectName("pageSub");
    titleBlock->addWidget(pageTitle);
    titleBlock->addWidget(pageSub);

    m_filterCombo = new QComboBox;
    m_filterCombo->setObjectName("filterCombo");
    m_filterCombo->addItems({"All Bills", "Paid", "Unpaid"});
    m_filterCombo->setFixedWidth(160);
    m_filterCombo->setFixedHeight(38);

    m_generateBtn = new QPushButton("＋  Generate Bill");
    m_generateBtn->setObjectName("genBtn");
    m_generateBtn->setFixedHeight(40);

    topLay->addLayout(titleBlock);
    topLay->addStretch();
    topLay->addWidget(m_filterCombo);
    topLay->addWidget(m_generateBtn);

    root->addWidget(topBar);

    auto *summaryStrip = new QFrame;
    summaryStrip->setObjectName("summaryStrip");
    summaryStrip->setFixedHeight(36);
    auto *sLay = new QHBoxLayout(summaryStrip);
    sLay->setContentsMargins(28, 0, 28, 0);

    m_summaryLabel = new QLabel;
    m_summaryLabel->setObjectName("summaryLabel");
    sLay->addWidget(m_summaryLabel);
    sLay->addStretch();
    root->addWidget(summaryStrip);

    m_table = new QTableWidget(0, COL_COUNT);
    m_table->setObjectName("billingTable");
    m_table->setHorizontalHeaderLabels({
        "Bill ID", "Patient Name", "Consultation",
        "Medicine", "Tests", "Total", "Status", "Action"
    });

    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_table->setColumnWidth(COL_ID,       80);
    m_table->setColumnWidth(COL_PATIENT,  160);
    m_table->setColumnWidth(COL_CONSULT,  130);
    m_table->setColumnWidth(COL_MEDICINE, 115);
    m_table->setColumnWidth(COL_TESTS,    105);
    m_table->setColumnWidth(COL_TOTAL,    120);
    m_table->setColumnWidth(COL_STATUS,   100);
    m_table->horizontalHeader()->setSectionResizeMode(COL_ACTION, QHeaderView::Stretch);

    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(false);
    m_table->setShowGrid(false);
    m_table->verticalHeader()->setDefaultSectionSize(54);
    m_table->setWordWrap(false);
    m_table->horizontalHeader()->setStretchLastSection(true);

    root->addWidget(m_table, 1);

    connect(m_generateBtn, &QPushButton::clicked,
            this, &BillingPage::openGenerateBillDialog);
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BillingPage::applyFilter);
}

void BillingPage::refresh()
{
    m_bills.clear();
    for (const Bill &b : m_hs->getBills()) {
        BillRecord r;
        r.billId          = b.getBillId();
        r.consultationFee = b.getConsultationFee();
        r.medicineFee     = b.getMedicineFee();
        r.testFee         = b.getTestFee();
        r.total           = b.getTotalAmount();
        r.paid            = b.getIsPaid();

        if (Patient *p = m_hs->findPatient(b.getPatientId()))
            r.patientName = QString::fromStdString(p->getName());
        else
            r.patientName = QString("Patient #%1").arg(b.getPatientId());

        r.date = QDate::fromString(
            QString::fromStdString(b.getBillDate()), "yyyy-MM-dd");
        m_bills.append(r);
    }

    int localNextId = 1;
    for (const auto &r : m_bills)
        if (r.billId >= localNextId) localNextId = r.billId + 1;

    refreshTable();
    emit dataChanged();
}

void BillingPage::refreshTable()
{
    const int filterIdx = m_filterCombo->currentIndex();

    QVector<BillRecord *> visible;
    for (auto &b : m_bills) {
        if (filterIdx == 0
            || (filterIdx == 1 && b.paid)
            || (filterIdx == 2 && !b.paid))
        {
            visible.append(&b);
        }
    }

    m_table->setRowCount(0);
    m_table->setRowCount(visible.size());

    for (int row = 0; row < visible.size(); ++row)
        populateRow(row, *visible[row]);

    int totalBills = m_bills.size();
    int paid   = 0; int unpaid = 0;
    double revenue = 0;
    for (auto &b : m_bills) {
        if (b.paid) { ++paid; revenue += b.total; }
        else ++unpaid;
    }
    m_summaryLabel->setText(
        QString("Total: <b>%1</b>  |  "
                "<span style='color:%2'>Paid: <b>%3</b></span>  |  "
                "<span style='color:%4'>Unpaid: <b>%5</b></span>  |  "
                "Revenue Collected: <b>PKR %6</b>")
        .arg(totalBills)
        .arg(Palette::SUCCESS).arg(paid)
        .arg(Palette::DANGER).arg(unpaid)
        .arg(revenue, 0, 'f', 2));
}

void BillingPage::populateRow(int row, const BillRecord &bill)
{
    auto cell = [&](int col, const QString &text,
                    Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter)
    {
        auto *item = new QTableWidgetItem(text);
        item->setTextAlignment(align);
        m_table->setItem(row, col, item);
        return item;
    };

    cell(COL_ID,       QString("#%1").arg(bill.billId),
                       Qt::AlignCenter | Qt::AlignVCenter);
    cell(COL_PATIENT,  bill.patientName);
    cell(COL_CONSULT,  QString("PKR %1").arg(bill.consultationFee, 0, 'f', 2),
                       Qt::AlignRight | Qt::AlignVCenter);
    cell(COL_MEDICINE, QString("PKR %1").arg(bill.medicineFee, 0, 'f', 2),
                       Qt::AlignRight | Qt::AlignVCenter);
    cell(COL_TESTS,    QString("PKR %1").arg(bill.testFee, 0, 'f', 2),
                       Qt::AlignRight | Qt::AlignVCenter);
    cell(COL_TOTAL,    QString("PKR %1").arg(bill.total, 0, 'f', 2),
                       Qt::AlignRight | Qt::AlignVCenter);

    auto *statusItem = cell(COL_STATUS,
                            bill.paid ? "✔  Paid" : "✘  Unpaid",
                            Qt::AlignCenter | Qt::AlignVCenter);
    statusItem->setForeground(QBrush(QColor(
        bill.paid ? Palette::SUCCESS : Palette::DANGER)));

    const QColor rowBg(bill.paid ? Palette::ROW_PAID : Palette::ROW_UNPAID);
    for (int c = 0; c < COL_ACTION; ++c) {
        if (auto *it = m_table->item(row, c))
            it->setBackground(rowBg);
    }

    auto *cell_widget = new QWidget;
    cell_widget->setObjectName("billActionCell");
    cell_widget->setStyleSheet(
        QString("QWidget#billActionCell { background: %1; }")
        .arg(bill.paid ? Palette::ROW_PAID : Palette::ROW_UNPAID));

    auto *lay = new QHBoxLayout(cell_widget);
    lay->setContentsMargins(8, 6, 8, 6);
    lay->setSpacing(8);

    if (!bill.paid) {
        auto *markBtn = new QPushButton("Mark Paid");
        markBtn->setObjectName("markPaidBtn");
        markBtn->setFixedHeight(30);
        markBtn->setStyleSheet(
            "QPushButton#markPaidBtn {"
            "  background-color: #E6F4EC;"
            "  color: #1B7A48;"
            "  border: 1.5px solid #1B7A48;"
            "  border-radius: 6px;"
            "  font-size: 12px;"
            "  font-weight: 700;"
            "  padding: 0 10px;"
            "}"
            "QPushButton#markPaidBtn:hover {"
            "  background-color: #1B7A48;"
            "  color: #FFFFFF;"
            "}"
        );

        const int id = bill.billId;
        connect(markBtn, &QPushButton::clicked,
                this, [this, id]{ markPaid(id); });

        lay->addWidget(markBtn);
    }

    auto *viewBtn = new QPushButton("View Receipt");
    viewBtn->setObjectName("viewRcptBtn");
    viewBtn->setFixedHeight(30);
    viewBtn->setStyleSheet(
        "QPushButton#viewRcptBtn {"
        "  background-color: #E8F1FB;"
        "  color: #1A6FC4;"
        "  border: 1.5px solid #1A6FC4;"
        "  border-radius: 6px;"
        "  font-size: 12px;"
        "  font-weight: 700;"
        "  padding: 0 10px;"
        "}"
        "QPushButton#viewRcptBtn:hover {"
        "  background-color: #1A6FC4;"
        "  color: #FFFFFF;"
        "}"
    );

    const int id = bill.billId;
    connect(viewBtn, &QPushButton::clicked,
            this, [this, id]{ viewReceipt(id); });

    lay->addWidget(viewBtn);
    lay->addStretch();

    m_table->setCellWidget(row, COL_ACTION, cell_widget);
}

BillRecord *BillingPage::findBill(int billId)
{
    for (auto &b : m_bills)
        if (b.billId == billId) return &b;
    return nullptr;
}

void BillingPage::markPaid(int billId)
{
    auto *b = findBill(billId);
    if (!b) return;

    const auto reply = QMessageBox::question(
        this,
        "Confirm Payment",
        QString("Mark Bill #%1 for <b>%2</b> as Paid?<br>"
                "Amount: <b>PKR %3</b>")
            .arg(billId)
            .arg(b->patientName)
            .arg(b->total, 0, 'f', 2),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes);

    if (reply == QMessageBox::Yes) {

        m_hs->markBillPaid(billId);
        refresh();   
    }
}

void BillingPage::viewReceipt(int billId)
{
    auto *b = findBill(billId);
    if (!b) return;

    ReceiptDialog dlg(*b, this);
    dlg.exec();
}

void BillingPage::openGenerateBillDialog()
{

    GenerateBillDialog dlg(m_hs, this);
    if (dlg.exec() != QDialog::Accepted) return;

    BillRecord r = dlg.result();
    int patId = dlg.selectedPatientId();

    if (patId < 0) {
        QMessageBox::warning(this, "No Patient Selected",
            "Please register patients before generating a bill.");
        return;
    }

    m_hs->generateBill(patId,
                       r.consultationFee,
                       r.medicineFee,
                       r.testFee);

    refresh();   
}

void BillingPage::applyFilter(int)
{
    refreshTable();
    emit dataChanged();
}

void BillingPage::applyStyles()
{
    setStyleSheet(QString(R"(
        BillingPage {
            background: %1;
        }

        /* ── Top bar ── */
        #topBar {
            background: %2;
            border-bottom: none;
        }
        #pageTitle {
            font-size: 20px;
            font-weight: 800;
            color: %4;
            background: transparent;
        }
        #pageSub {
            font-size: 12px;
            color: %5;
            background: transparent;
        }

        /* ── Filter combo ── */
        #filterCombo {
            background: #FFFFFF;
            border: 1.5px solid %3;
            border-radius: 8px;
            padding: 4px 12px;
            font-size: 13px;
            font-weight: 600;
            color: %4;
        }
        #filterCombo::drop-down { border: none; width: 24px; }
        #filterCombo:hover { border-color: %6; }

        /* ── Generate button ── */
        #genBtn {
            background: %6;
            color: #FFFFFF;
            border: none;
            border-radius: 9px;
            font-size: 14px;
            font-weight: 700;
            padding: 0 22px;
        }
        #genBtn:hover { background: %7; }

        /* ── Summary strip ── */
        #summaryStrip {
            background: %2;
            border-bottom: 1px solid %3;
        }
        #summaryLabel {
            font-size: 12px;
            color: %4;
            background: transparent;
        }

        /* ── Table ── */
        #billingTable {
            background: #FFFFFF;
            border: none;
            gridline-color: transparent;
            font-size: 13px;
            color: %4;
        }
        #billingTable QHeaderView::section {
            background: %9;
            color: %10;
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 1px;
            text-transform: uppercase;
            padding: 10px 12px;
            border: none;
            border-right: 1px solid rgba(255,255,255,0.1);
        }
        #billingTable::item {
            padding: 0 12px;
            border-bottom: 1px solid %3;
        }
        #billingTable::item:selected {
            background: %11;
            color: %4;
        }

        /* ── Action buttons ── */
        #markPaidBtn {
            background: %12;
            color: %13;
            border: 1.5px solid %13;
            border-radius: 6px;
            font-size: 12px;
            font-weight: 700;
            padding: 0 10px;
        }
        #markPaidBtn:hover {
            background: %13;
            color: #FFFFFF;
        }
        #viewRcptBtn {
            background: %14;
            color: %6;
            border: 1.5px solid %6;
            border-radius: 6px;
            font-size: 12px;
            font-weight: 700;
            padding: 0 10px;
        }
        #viewRcptBtn:hover {
            background: %6;
            color: #FFFFFF;
        }
    )").arg(
        Palette::BG_PAGE,      
        Palette::BG_CARD,      
        Palette::BORDER,       
        Palette::TEXT_PRIMARY, 
        Palette::TEXT_MUTED,   
        Palette::ACCENT,       
        Palette::ACCENT_HOVER, 
        Palette::ACCENT_LIGHT, 
        Palette::HEADER_BG,    
        Palette::HEADER_FG,    
        Palette::ACCENT_LIGHT, 
        Palette::SUCCESS_BG,   
        Palette::SUCCESS,      
        Palette::ACCENT_LIGHT  
    ));
}
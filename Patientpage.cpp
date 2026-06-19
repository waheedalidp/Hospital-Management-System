#include "PatientPage.h"

enum Col {
    COL_ID      = 0,
    COL_NAME    = 1,
    COL_AGE     = 2,
    COL_PHONE   = 3,
    COL_DISEASE = 4,
    COL_DATE    = 5,
    COL_ACTION  = 6,
    COL_COUNT   = 7
};

PatientPage::PatientPage(HospitalSystem *hs, QWidget *parent)
    : QWidget(parent), hs_(hs)
{
    buildUi();
    applyStyles();
    refreshTable();   
}

void PatientPage::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(0);

    auto *topRow = new QHBoxLayout();

    auto *title = new QLabel("Patients");
    title->setStyleSheet("font-size:22px; font-weight:800; color:#1A202C;");

    countLabel_ = new QLabel("Loading...");
    countLabel_->setStyleSheet("font-size:13px; color:#718096;");

    addBtn_ = new QPushButton("＋  Add Patient");
    addBtn_->setObjectName("addPatientBtn");
    addBtn_->setFixedHeight(40);
    addBtn_->setCursor(Qt::PointingHandCursor);

    topRow->addWidget(title);
    topRow->addSpacing(14);
    topRow->addWidget(countLabel_);
    topRow->addStretch();
    topRow->addWidget(addBtn_);
    root->addLayout(topRow);
    root->addSpacing(18);

    table_ = new QTableWidget(0, COL_COUNT);
    table_->setObjectName("patientTable");
    table_->setHorizontalHeaderLabels(
        {"ID", "Name", "Age", "Phone", "Disease", "Admitted", "Action"});

    table_->horizontalHeader()->setSectionResizeMode(COL_NAME,    QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(COL_DISEASE, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(COL_ACTION,  QHeaderView::Fixed);
    table_->setColumnWidth(COL_ID,      60);
    table_->setColumnWidth(COL_AGE,     55);
    table_->setColumnWidth(COL_PHONE,  140);
    table_->setColumnWidth(COL_DATE,   130);
    table_->setColumnWidth(COL_ACTION,  90);

    table_->verticalHeader()->setVisible(false);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setAlternatingRowColors(true);
    table_->setShowGrid(false);
    table_->verticalHeader()->setDefaultSectionSize(46);

    root->addWidget(table_, 1);

    connect(addBtn_, &QPushButton::clicked, this, &PatientPage::onAddClicked);
}

void PatientPage::refreshTable()
{
    table_->setRowCount(0);   

    const auto &patients = hs_->getPatients();

    for (const Patient &p : patients) {
        int row = table_->rowCount();
        table_->insertRow(row);

        auto cell = [&](int col, const QString &text,
                        Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter)
        {
            auto *item = new QTableWidgetItem(text);
            item->setTextAlignment(align);
            table_->setItem(row, col, item);
        };

        cell(COL_ID,      QString::number(p.getId()),
                          Qt::AlignCenter | Qt::AlignVCenter);
        cell(COL_NAME,    QString::fromStdString(p.getName()));
        cell(COL_AGE,     QString::number(p.getAge()),
                          Qt::AlignCenter | Qt::AlignVCenter);
        cell(COL_PHONE,   QString::fromStdString(p.getPhone()));
        cell(COL_DISEASE, QString::fromStdString(p.getDisease()));
        cell(COL_DATE,    QString::fromStdString(p.getAdmissionDate()));

        auto *delBtn = new QPushButton("Delete");
        delBtn->setObjectName("deleteBtn");
        delBtn->setFixedHeight(30);
        delBtn->setProperty("patientId", p.getId());
        connect(delBtn, &QPushButton::clicked,
                this, &PatientPage::onDeleteClicked);
        table_->setCellWidget(row, COL_ACTION, delBtn);
    }

    countLabel_->setText(
        QString("Showing %1 patient(s)").arg(patients.size()));
}

void PatientPage::onAddClicked()
{
    PatientDialog dlg(PatientDialog::AddMode, this);
    if (dlg.exec() != QDialog::Accepted)
        return;   

    PatientRecord rec = dlg.getPatientData();

    Patient newPat(
        0,
        rec.name.toStdString(),
        rec.age,
        rec.phone.toStdString(),
        rec.disease.toStdString(),
        rec.admissionDate.toStdString()
    );

    hs_->addPatient(newPat);   
    refreshTable();
    emit dataChanged();
}

void PatientPage::onDeleteClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int patId = btn->property("patientId").toInt();

    Patient *p = hs_->findPatient(patId);
    if (!p) return;

    QString name = QString::fromStdString(p->getName());

    auto reply = QMessageBox::warning(
        this,
        "Delete Patient",
        QString("Permanently delete <b>%1</b> (ID %2)?<br>This cannot be undone.")
            .arg(name).arg(patId),
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);   

    if (reply != QMessageBox::Yes)
        return;

    hs_->removePatient(patId);   
    refreshTable();
    emit dataChanged();
}

void PatientPage::applyStyles()
{
    setStyleSheet(R"(
        QWidget { background: #F0F4F8; }

        QPushButton#addPatientBtn {
            background: #3B82F6;
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 700;
            padding: 0 20px;
        }
        QPushButton#addPatientBtn:hover { background: #2563EB; }

        QTableWidget#patientTable {
            background: #FFFFFF;
            border: 1px solid #E2E8F0;
            border-radius: 10px;
            gridline-color: transparent;
            font-size: 13px;
        }
        QHeaderView::section {
            background: #F8FAFC;
            color: #374151;
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 0.5px;
            padding: 10px 8px;
            border: none;
            border-bottom: 2px solid #E2E8F0;
        }
        QTableWidget::item {
            padding: 0 8px;
            border-bottom: 1px solid #F1F5F9;
        }
        QTableWidget::item:selected {
            background: #EFF6FF;
            color: #1E40AF;
        }
        QTableWidget::item:alternate { background: #F8FAFC; }

        QPushButton#deleteBtn {
            background: #FEE2E2;
            color: #DC2626;
            border: none;
            border-radius: 6px;
            font-size: 12px;
            font-weight: 700;
            margin: 4px 6px;
        }
        QPushButton#deleteBtn:hover {
            background: #DC2626;
            color: #FFFFFF;
        }
    )");
}
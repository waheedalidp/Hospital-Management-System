#include "DoctorPage.h"
#include <QStringList>

enum DCol {
    DCOL_ID      = 0,
    DCOL_NAME    = 1,
    DCOL_AGE     = 2,
    DCOL_PHONE   = 3,
    DCOL_SPEC    = 4,
    DCOL_FEE     = 5,
    DCOL_SLOTS   = 6,
    DCOL_ACTION  = 7,
    DCOL_COUNT   = 8
};

DoctorDialog::DoctorDialog(QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    setFixedWidth(460);
    setWindowTitle("Add New Doctor");

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    buildUi();
    applyStyles();
}

void DoctorDialog::buildUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *card = new QFrame();
    card->setObjectName("dialogCard");
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 28, 32, 28);
    cardLayout->setSpacing(0);

    auto *titleLabel = new QLabel("Add New Doctor");
    titleLabel->setObjectName("dialogTitle");

    auto *subLabel = new QLabel("Fill in the details below to register a new doctor.");
    subLabel->setObjectName("dialogSub");

    errorLabel_ = new QLabel();
    errorLabel_->setObjectName("errorLabel");
    errorLabel_->setVisible(false);
    errorLabel_->setWordWrap(true);

    cardLayout->addWidget(titleLabel);
    cardLayout->addSpacing(4);
    cardLayout->addWidget(subLabel);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(errorLabel_);

    auto makeLabel = [](const QString &text) -> QLabel* {
        auto *lbl = new QLabel(text);
        lbl->setStyleSheet(
            "color:#374151; font-size:12px; font-weight:700; "
            "letter-spacing:0.5px; text-transform:uppercase;");
        return lbl;
    };

    cardLayout->addWidget(makeLabel("Full Name"));
    cardLayout->addSpacing(6);
    nameEdit_ = new QLineEdit;
    nameEdit_->setObjectName("formField");
    nameEdit_->setPlaceholderText("e.g. Dr. Sarah Khan");
    cardLayout->addWidget(nameEdit_);
    cardLayout->addSpacing(14);

    auto *twoCol = new QHBoxLayout;
    twoCol->setSpacing(14);

    auto *ageCol = new QVBoxLayout;
    ageCol->setSpacing(6);
    ageCol->addWidget(makeLabel("Age"));
    ageSpin_ = new QSpinBox;
    ageSpin_->setObjectName("formSpin");
    ageSpin_->setRange(1, 120);
    ageSpin_->setValue(35);
    ageCol->addWidget(ageSpin_);

    auto *phoneCol = new QVBoxLayout;
    phoneCol->setSpacing(6);
    phoneCol->addWidget(makeLabel("Phone Number"));
    phoneEdit_ = new QLineEdit;
    phoneEdit_->setObjectName("formField");
    phoneEdit_->setPlaceholderText("e.g. 03001234567");
    phoneCol->addWidget(phoneEdit_);

    twoCol->addLayout(ageCol);
    twoCol->addLayout(phoneCol);
    cardLayout->addLayout(twoCol);
    cardLayout->addSpacing(14);

    cardLayout->addWidget(makeLabel("Specialization"));
    cardLayout->addSpacing(6);
    specEdit_ = new QLineEdit;
    specEdit_->setObjectName("formField");
    specEdit_->setPlaceholderText("e.g. Cardiologist, Neurologist…");
    cardLayout->addWidget(specEdit_);
    cardLayout->addSpacing(14);

    cardLayout->addWidget(makeLabel("Consultation Fee (PKR)"));
    cardLayout->addSpacing(6);
    feeSpin_ = new QDoubleSpinBox;
    feeSpin_->setObjectName("formSpin");
    feeSpin_->setRange(0.01, 999999.0);
    feeSpin_->setDecimals(2);
    feeSpin_->setSingleStep(100.0);
    feeSpin_->setValue(1500.0);
    feeSpin_->setPrefix("PKR  ");
    cardLayout->addWidget(feeSpin_);
    cardLayout->addSpacing(28);

    auto *btnRow = new QHBoxLayout;
    btnRow->setSpacing(12);

    cancelBtn_ = new QPushButton("Cancel");
    cancelBtn_->setObjectName("cancelBtn");
    cancelBtn_->setFixedHeight(42);
    cancelBtn_->setCursor(Qt::PointingHandCursor);

    saveBtn_ = new QPushButton("Add Doctor");
    saveBtn_->setObjectName("saveBtn");
    saveBtn_->setFixedHeight(42);
    saveBtn_->setCursor(Qt::PointingHandCursor);

    btnRow->addWidget(cancelBtn_, 1);
    btnRow->addWidget(saveBtn_, 2);
    cardLayout->addLayout(btnRow);

    rootLayout->addWidget(card);

    connect(saveBtn_,   &QPushButton::clicked, this, &DoctorDialog::onSaveClicked);
    connect(cancelBtn_, &QPushButton::clicked, this, &QDialog::reject);
}

bool DoctorDialog::validateAll()
{
    QStringList errors;

    if (nameEdit_->text().trimmed().isEmpty()) {
        nameEdit_->setStyleSheet(
            "QLineEdit { background:#FEF2F2; border:1.5px solid #EF4444;"
            "border-radius:8px; padding:10px 14px; font-size:14px; }");
        errors << "Name is required.";
    } else {
        nameEdit_->setStyleSheet("");
    }

    {
        QString digits = phoneEdit_->text().trimmed();
        digits.remove(QRegularExpression("[\\s\\-]"));
        if (digits.isEmpty() || digits.length() != 11) {
            phoneEdit_->setStyleSheet(
                "QLineEdit { background:#FEF2F2; border:1.5px solid #EF4444;"
                "border-radius:8px; padding:10px 14px; font-size:14px; }");
            errors << "Phone must be exactly 11 digits.";
        } else {
            phoneEdit_->setStyleSheet("");
        }
    }

    if (specEdit_->text().trimmed().isEmpty()) {
        specEdit_->setStyleSheet(
            "QLineEdit { background:#FEF2F2; border:1.5px solid #EF4444;"
            "border-radius:8px; padding:10px 14px; font-size:14px; }");
        errors << "Specialization is required.";
    } else {
        specEdit_->setStyleSheet("");
    }

    if (feeSpin_->value() <= 0.0) {
        errors << "Consultation fee must be greater than zero.";
    }

    if (!errors.isEmpty()) {
        errorLabel_->setText("⚠  " + errors.join("  ·  "));
        errorLabel_->setVisible(true);
        return false;
    }
    errorLabel_->setVisible(false);
    return true;
}

void DoctorDialog::onSaveClicked()
{
    if (validateAll()) accept();
}

DoctorRecord DoctorDialog::getData() const
{
    DoctorRecord r;
    r.name            = nameEdit_->text().trimmed();
    r.age             = ageSpin_->value();
    r.phone           = phoneEdit_->text().trimmed();
    r.specialization  = specEdit_->text().trimmed();
    r.consultationFee = feeSpin_->value();
    return r;
}

void DoctorDialog::applyStyles()
{
    setStyleSheet(R"(
        QDialog {
            background:#FFFFFF;
            border-radius:16px;
            border: 1px solid #E2E8F0;
        }
        QFrame#dialogCard { background:#FFFFFF; border-radius:16px; }
        QLabel#dialogTitle {
            font-size:20px; font-weight:800;
            color:#111827; letter-spacing:-0.3px;
        }
        QLabel#dialogSub { font-size:13px; color:#6B7280; }
        QLabel#errorLabel {
            background:#FEF2F2; color:#B91C1C;
            border:1px solid #FECACA; border-radius:8px;
            padding:10px 14px; font-size:13px; font-weight:600;
            margin-bottom:8px;
        }
        QLineEdit#formField {
            background:#F8FAFC; border:1.5px solid #E2E8F0;
            border-radius:8px; padding:10px 14px;
            font-size:14px; color:#1A202C;
        }
        QLineEdit#formField:focus {
            border-color:#3B82F6; background:#FFFFFF;
        }
        QSpinBox#formSpin, QDoubleSpinBox#formSpin {
            background:#F8FAFC; border:1.5px solid #E2E8F0;
            border-radius:8px; padding:10px 14px;
            font-size:14px; color:#1A202C;
        }
        QSpinBox#formSpin:focus, QDoubleSpinBox#formSpin:focus {
            border-color:#3B82F6; background:#FFFFFF;
        }
        QSpinBox#formSpin::up-button, QSpinBox#formSpin::down-button,
        QDoubleSpinBox#formSpin::up-button, QDoubleSpinBox#formSpin::down-button {
            width:22px; border:none; background:#EDF2F7; border-radius:4px;
        }
        QPushButton#cancelBtn {
            background:#F1F5F9; color:#475569;
            border:none; border-radius:8px;
            font-size:14px; font-weight:600;
        }
        QPushButton#cancelBtn:hover { background:#E2E8F0; color:#1E293B; }
        QPushButton#saveBtn {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                                        stop:0 #3B82F6, stop:1 #2563EB);
            color:#FFFFFF; border:none; border-radius:8px;
            font-size:14px; font-weight:700;
        }
        QPushButton#saveBtn:hover {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                                        stop:0 #2563EB, stop:1 #1D4ED8);
        }
    )");
}

DoctorPage::DoctorPage(HospitalSystem *hs, QWidget *parent)
    : QWidget(parent), hs_(hs)
{
    buildUi();
    applyStyles();
    refreshTable();
}

void DoctorPage::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(0);

    auto *topRow = new QHBoxLayout();

    auto *title = new QLabel("Doctors");
    title->setStyleSheet("font-size:22px; font-weight:800; color:#1A202C;");

    countLabel_ = new QLabel("Loading...");
    countLabel_->setStyleSheet("font-size:13px; color:#718096;");

    addBtn_ = new QPushButton("＋  Add Doctor");
    addBtn_->setObjectName("addDoctorBtn");
    addBtn_->setFixedHeight(40);
    addBtn_->setCursor(Qt::PointingHandCursor);

    topRow->addWidget(title);
    topRow->addSpacing(14);
    topRow->addWidget(countLabel_);
    topRow->addStretch();
    topRow->addWidget(addBtn_);
    root->addLayout(topRow);
    root->addSpacing(18);

    table_ = new QTableWidget(0, DCOL_COUNT);
    table_->setObjectName("doctorTable");
    table_->setHorizontalHeaderLabels(
        {"ID", "Name", "Age", "Phone", "Specialization",
         "Fee (PKR)", "Available Slots", "Action"});

    table_->horizontalHeader()->setSectionResizeMode(DCOL_NAME,   QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(DCOL_SPEC,   QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(DCOL_ACTION, QHeaderView::Fixed);
    table_->setColumnWidth(DCOL_ID,     60);
    table_->setColumnWidth(DCOL_AGE,    55);
    table_->setColumnWidth(DCOL_PHONE, 140);
    table_->setColumnWidth(DCOL_FEE,   110);
    table_->setColumnWidth(DCOL_SLOTS, 110);
    table_->setColumnWidth(DCOL_ACTION, 90);

    table_->verticalHeader()->setVisible(false);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setAlternatingRowColors(true);
    table_->setShowGrid(false);
    table_->verticalHeader()->setDefaultSectionSize(46);

    root->addWidget(table_, 1);

    connect(addBtn_, &QPushButton::clicked, this, &DoctorPage::onAddClicked);
}

void DoctorPage::refreshTable()
{
    table_->setRowCount(0);

    const auto &doctors = hs_->getDoctors();

    for (const Doctor &d : doctors) {
        int row = table_->rowCount();
        table_->insertRow(row);

        auto cell = [&](int col, const QString &text,
                        Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter)
        {
            auto *item = new QTableWidgetItem(text);
            item->setTextAlignment(align);
            table_->setItem(row, col, item);
        };

        cell(DCOL_ID,    QString::number(d.getId()),
             Qt::AlignCenter | Qt::AlignVCenter);
        cell(DCOL_NAME,  QString::fromStdString(d.getName()));
        cell(DCOL_AGE,   QString::number(d.getAge()),
             Qt::AlignCenter | Qt::AlignVCenter);
        cell(DCOL_PHONE, QString::fromStdString(d.getPhone()));
        cell(DCOL_SPEC,  QString::fromStdString(d.getSpecialization()));
        cell(DCOL_FEE,   QString("PKR %1").arg(d.getConsultationFee(), 0, 'f', 2),
             Qt::AlignRight | Qt::AlignVCenter);

        cell(DCOL_SLOTS, "---");

        auto *delBtn = new QPushButton("Delete");
        delBtn->setObjectName("deleteBtn");
        delBtn->setFixedHeight(30);
        delBtn->setProperty("doctorId", d.getId());
        connect(delBtn, &QPushButton::clicked,
                this, &DoctorPage::onDeleteClicked);
        table_->setCellWidget(row, DCOL_ACTION, delBtn);
    }

    countLabel_->setText(
        QString("Showing %1 doctor(s)").arg(doctors.size()));
}

void DoctorPage::onAddClicked()
{
    DoctorDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    DoctorRecord rec = dlg.getData();

    Doctor newDoc(
        0,
        rec.name.toStdString(),
        rec.age,
        rec.phone.toStdString(),
        rec.specialization.toStdString(),
        rec.consultationFee
        );

    hs_->addDoctor(newDoc);
    refreshTable();
    emit dataChanged();
}

void DoctorPage::onDeleteClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int docId = btn->property("doctorId").toInt();

    Doctor *d = hs_->findDoctor(docId);
    if (!d) return;

    QString name = QString::fromStdString(d->getName());

    auto reply = QMessageBox::warning(
        this,
        "Delete Doctor",
        QString("Permanently delete <b>Dr. %1</b> (ID %2)?<br>This cannot be undone.")
            .arg(name).arg(docId),
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if (reply != QMessageBox::Yes)
        return;

    hs_->removeDoctor(docId);
    refreshTable();
    emit dataChanged();
}

void DoctorPage::applyStyles()
{
    setStyleSheet(R"(
        QWidget { background: #F0F4F8; }

        QPushButton#addDoctorBtn {
            background: #3B82F6;
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 700;
            padding: 0 20px;
        }
        QPushButton#addDoctorBtn:hover { background: #2563EB; }

        QTableWidget#doctorTable {
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
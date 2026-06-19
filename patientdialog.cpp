#include "patientdialog.h"

#include <QGraphicsDropShadowEffect>
#include <QDate>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

ValidatedLineEdit::ValidatedLineEdit(const QString &placeholder, QWidget *parent)
    : QLineEdit(parent)
{
    setPlaceholderText(placeholder);
    baseStyle_ =
        "QLineEdit {"
        "  background: #F8FAFC;"
        "  border: 1.5px solid #E2E8F0;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "  font-size: 14px;"
        "  color: #1A202C;"
        "}"
        "QLineEdit:focus {"
        "  border-color: #3B82F6;"
        "  background: #FFFFFF;"
        "}";
    setStyleSheet(baseStyle_);
}

bool ValidatedLineEdit::validate()
{
    if (text().trimmed().isEmpty()) {
        setStyleSheet(
            "QLineEdit {"
            "  background: #FEF2F2;"
            "  border: 1.5px solid #EF4444;"
            "  border-radius: 8px;"
            "  padding: 10px 14px;"
            "  font-size: 14px;"
            "  color: #1A202C;"
            "}"
            "QLineEdit:focus {"
            "  border-color: #EF4444;"
            "  background: #FEF2F2;"
            "}"
        );
        return false;
    }
    clearError();
    return true;
}

void ValidatedLineEdit::clearError()
{
    setStyleSheet(baseStyle_);
}

PatientDialog::PatientDialog(Mode mode, QWidget *parent)
    : QDialog(parent), mode_(mode)
{
    setModal(true);
    setFixedWidth(480);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    buildUi();
    applyStyles();
}

void PatientDialog::setPatientData(const PatientRecord &rec)
{
    editingId_ = rec.id;
    nameEdit_->setText(rec.name);
    ageSpinBox_->setValue(rec.age);
    phoneEdit_->setText(rec.phone);
    diseaseEdit_->setText(rec.disease);
    if (!rec.admissionDate.isEmpty())
        admissionDateEdit_->setDate(QDate::fromString(rec.admissionDate, "yyyy-MM-dd"));
}

PatientRecord PatientDialog::getPatientData() const
{
    PatientRecord rec;
    rec.id            = editingId_;
    rec.name          = nameEdit_->text().trimmed();
    rec.age           = ageSpinBox_->value();
    rec.phone         = phoneEdit_->text().trimmed();
    rec.disease       = diseaseEdit_->text().trimmed();
    rec.admissionDate = admissionDateEdit_->date().toString("yyyy-MM-dd");
    return rec;
}

void PatientDialog::buildUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *card = new QFrame();
    card->setObjectName("dialogCard");
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 28, 32, 28);
    cardLayout->setSpacing(0);

    QString titleText = (mode_ == AddMode) ? "Add New Patient" : "Edit Patient";
    QString subText   = (mode_ == AddMode) ? "Fill in the details below to register a new patient."
                                           : "Update the patient information below.";
    auto *titleLabel = new QLabel(titleText);
    titleLabel->setObjectName("dialogTitle");

    auto *subLabel = new QLabel(subText);
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

    auto makeFieldLabel = [](const QString &text) -> QLabel* {
        QLabel *lbl = new QLabel(text);
        lbl->setStyleSheet(
            "color: #374151; font-size: 12px; font-weight: 700; "
            "letter-spacing: 0.5px; text-transform: uppercase;"
        );
        return lbl;
    };

    cardLayout->addWidget(makeFieldLabel("Full Name"));
    cardLayout->addSpacing(6);
    nameEdit_ = new ValidatedLineEdit("e.g. Sarah Johnson");
    cardLayout->addWidget(nameEdit_);
    cardLayout->addSpacing(14);

    auto *twoColRow = new QHBoxLayout();
    twoColRow->setSpacing(14);

    auto *ageCol = new QVBoxLayout();
    ageCol->setSpacing(6);
    ageCol->addWidget(makeFieldLabel("Age"));
    ageSpinBox_ = new QSpinBox();
    ageSpinBox_->setRange(1, 120);   
    ageSpinBox_->setValue(30);
    ageSpinBox_->setObjectName("ageSpinBox");
    ageCol->addWidget(ageSpinBox_);

    auto *phoneCol = new QVBoxLayout();
    phoneCol->setSpacing(6);
    phoneCol->addWidget(makeFieldLabel("Phone Number"));
    phoneEdit_ = new ValidatedLineEdit("e.g. +92-300-1234567");
    phoneCol->addWidget(phoneEdit_);

    twoColRow->addLayout(ageCol);
    twoColRow->addLayout(phoneCol);
    cardLayout->addLayout(twoColRow);
    cardLayout->addSpacing(14);

    cardLayout->addWidget(makeFieldLabel("Diagnosis / Disease"));
    cardLayout->addSpacing(6);
    diseaseEdit_ = new ValidatedLineEdit("e.g. Hypertension, Diabetes…");
    cardLayout->addWidget(diseaseEdit_);
    cardLayout->addSpacing(14);

    cardLayout->addWidget(makeFieldLabel("Admission Date"));
    cardLayout->addSpacing(6);
    admissionDateEdit_ = new QDateEdit(QDate::currentDate());
    admissionDateEdit_->setObjectName("admissionDate");
    admissionDateEdit_->setCalendarPopup(true);
    admissionDateEdit_->setDisplayFormat("MMMM d, yyyy");
    cardLayout->addWidget(admissionDateEdit_);
    cardLayout->addSpacing(28);

    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    cancelBtn_ = new QPushButton("Cancel");
    cancelBtn_->setObjectName("cancelBtn");
    cancelBtn_->setFixedHeight(42);
    cancelBtn_->setCursor(Qt::PointingHandCursor);

    saveBtn_ = new QPushButton((mode_ == AddMode) ? "Add Patient" : "Save Changes");
    saveBtn_->setObjectName("saveBtn");
    saveBtn_->setFixedHeight(42);
    saveBtn_->setCursor(Qt::PointingHandCursor);

    btnRow->addWidget(cancelBtn_, 1);
    btnRow->addWidget(saveBtn_, 2);

    cardLayout->addLayout(btnRow);
    rootLayout->addWidget(card);

    connect(saveBtn_,   &QPushButton::clicked, this, &PatientDialog::onSaveClicked);
    connect(cancelBtn_, &QPushButton::clicked, this, &QDialog::reject);

    connect(nameEdit_,    &QLineEdit::textChanged, nameEdit_,    &ValidatedLineEdit::clearError);
    connect(phoneEdit_,   &QLineEdit::textChanged, phoneEdit_,   &ValidatedLineEdit::clearError);
    connect(diseaseEdit_, &QLineEdit::textChanged, diseaseEdit_, &ValidatedLineEdit::clearError);
}

bool PatientDialog::validateAll()
{
    bool ok = true;
    ok = nameEdit_->validate()    && ok;
    ok = phoneEdit_->validate()   && ok;
    ok = diseaseEdit_->validate() && ok;

    if (ageSpinBox_->value() < 1 || ageSpinBox_->value() > 120) {
        ageSpinBox_->setStyleSheet(
            "QSpinBox { background: #FEF2F2; border: 1.5px solid #EF4444;"
            "border-radius: 8px; padding: 10px 14px; font-size: 14px; }"
        );
        ok = false;
    }

    {
        QString digitsOnly = phoneEdit_->text().trimmed();
        digitsOnly.remove(QRegularExpression("[\\s\\-]"));
        if (!digitsOnly.isEmpty() && digitsOnly.length() != 11) {
            phoneEdit_->setStyleSheet(
                "QLineEdit { background: #FEF2F2; border: 1.5px solid #EF4444;"
                "border-radius: 8px; padding: 10px 14px; font-size: 14px; }"
                "QLineEdit:focus { border-color: #EF4444; background: #FEF2F2; }"
            );
            ok = false;
        }
    }

    if (!ok) {
        errorLabel_->setText(
            "⚠  Please fix the highlighted fields. "
            "Age: 1–120. Phone: 11 digits. All fields required.");
        errorLabel_->setVisible(true);
    } else {
        errorLabel_->setVisible(false);
    }
    return ok;
}

void PatientDialog::onSaveClicked()
{
    if (validateAll()) accept();
}

void PatientDialog::applyStyles()
{
    setStyleSheet(R"(
        QDialog {
            background: #FFFFFF;
            border-radius: 16px;
        }
        QFrame#dialogCard {
            background: #FFFFFF;
        }
        QLabel#dialogTitle {
            font-size: 20px;
            font-weight: 800;
            color: #111827;
            letter-spacing: -0.3px;
        }
        QLabel#dialogSub {
            font-size: 13px;
            color: #6B7280;
        }
        QLabel#errorLabel {
            background: #FEF2F2;
            color: #B91C1C;
            border: 1px solid #FECACA;
            border-radius: 8px;
            padding: 10px 14px;
            font-size: 13px;
            font-weight: 600;
            margin-bottom: 8px;
        }
        QSpinBox#ageSpinBox {
            background: #F8FAFC;
            border: 1.5px solid #E2E8F0;
            border-radius: 8px;
            padding: 10px 14px;
            font-size: 14px;
            color: #1A202C;
        }
        QSpinBox#ageSpinBox:focus {
            border-color: #3B82F6;
            background: #FFFFFF;
        }
        QSpinBox#ageSpinBox::up-button, QSpinBox#ageSpinBox::down-button {
            width: 22px;
            border: none;
            background: #EDF2F7;
            border-radius: 4px;
        }
        QDateEdit#admissionDate {
            background: #F8FAFC;
            border: 1.5px solid #E2E8F0;
            border-radius: 8px;
            padding: 10px 14px;
            font-size: 14px;
            color: #1A202C;
        }
        QDateEdit#admissionDate:focus {
            border-color: #3B82F6;
            background: #FFFFFF;
        }
        QDateEdit#admissionDate::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: right center;
            width: 30px;
            border: none;
            background: transparent;
        }
        QPushButton#cancelBtn {
            background: #F1F5F9;
            color: #475569;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton#cancelBtn:hover {
            background: #E2E8F0;
            color: #1E293B;
        }
        QPushButton#saveBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 #10B981, stop:1 #059669);
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton#saveBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 #059669, stop:1 #047857);
        }
        QPushButton#saveBtn:pressed {
            background: #047857;
        }
    )");

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(40);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 45));
    setGraphicsEffect(shadow);
}
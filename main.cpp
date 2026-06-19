#include "mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <QRect>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    app.setApplicationName("Hospital Management System");
    app.setOrganizationName("MedCore");
    app.setApplicationVersion("1.0.0");

    app.setStyleSheet(R"(

/* ──────────────────────────────────────────────────────────────
   1. GLOBAL — font, background, base text colour
   ────────────────────────────────────────────────────────────── */
* {
    font-family: "Segoe UI", "San Francisco", "Helvetica Neue", Arial, sans-serif;
    font-size: 10pt;
    color: #2D3748;
}

QMainWindow, QWidget {
    background-color: #F0F4F8;
}

/* ──────────────────────────────────────────────────────────────
   2. TOP BAR
   ────────────────────────────────────────────────────────────── */
QWidget#topBar {
    background-color: #FFFFFF;
    border-bottom: 1px solid #E2E8F0;
}

QLabel#titleLabel {
    font-size: 13pt;
    font-weight: 700;
    color: #1A202C;
    letter-spacing: 0.3px;
}

QLabel#dateLabel {
    font-size: 9pt;
    color: #718096;
}

/* ──────────────────────────────────────────────────────────────
   2. SIDEBAR — dark navy, white text
   ────────────────────────────────────────────────────────────── */
QWidget#sidebar {
    background-color: #1A2744;
    border-right: 1px solid #111D33;
}

QLabel#logoLabel {
    font-size: 15pt;
    font-weight: 800;
    color: #FFFFFF;
    letter-spacing: 1.5px;
    padding-left: 20px;
}

QFrame#sidebarDivider {
    color: #2A3A5C;
    margin: 0 16px;
}

QLabel#versionLabel {
    color: #4A6278;
    font-size: 8pt;
    padding-left: 20px;
}

/* Sidebar button — default state */
QPushButton#sidebarBtn {
    background-color: transparent;
    color: #94A3B8;
    border: none;
    border-radius: 0px;
    text-align: left;
    padding-left: 20px;
    font-size: 10pt;
    font-weight: 500;
}

QPushButton#sidebarBtn:hover {
    background-color: #243358;
    color: #E2E8F0;
}

/* Sidebar button — active/selected state */
QPushButton#sidebarBtn[active="true"] {
    background-color: #2563EB;
    color: #FFFFFF;
    font-weight: 700;
    border-left: 4px solid #93C5FD;
    padding-left: 16px;
}

QPushButton#sidebarBtn[active="true"]:hover {
    background-color: #1D4ED8;
}

/* ──────────────────────────────────────────────────────────────
   3. BUTTONS — Primary (blue), Danger (red), Success (green)
   ────────────────────────────────────────────────────────────── */

/* Primary — any button with objectName "primaryBtn" or "addPatientBtn"
   or "genBtn" (billing generate) */
QPushButton#primaryBtn,
QPushButton#addPatientBtn,
QPushButton#genBtn,
QPushButton#saveBtn {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 6px 20px;
    font-size: 10pt;
    font-weight: 700;
}
QPushButton#primaryBtn:hover,
QPushButton#addPatientBtn:hover,
QPushButton#genBtn:hover,
QPushButton#saveBtn:hover {
    background-color: #1D4ED8;
}
QPushButton#primaryBtn:pressed,
QPushButton#addPatientBtn:pressed,
QPushButton#genBtn:pressed,
QPushButton#saveBtn:pressed {
    background-color: #1E40AF;
}

/* Danger — delete, cancel appointment */
QPushButton#deleteBtn,
QPushButton#dangerBtn,
QPushButton#cancelAppointmentBtn {
    background-color: #FEE2E2;
    color: #DC2626;
    border: 1.5px solid #DC2626;
    border-radius: 8px;
    padding: 6px 16px;
    font-size: 10pt;
    font-weight: 700;
}
QPushButton#deleteBtn:hover,
QPushButton#dangerBtn:hover,
QPushButton#cancelAppointmentBtn:hover {
    background-color: #DC2626;
    color: #FFFFFF;
}

/* Success — mark paid, complete appointment */
QPushButton#markPaidBtn,
QPushButton#successBtn,
QPushButton#completeBtn {
    background-color: #D1FAE5;
    color: #065F46;
    border: 1.5px solid #059669;
    border-radius: 8px;
    padding: 6px 16px;
    font-size: 10pt;
    font-weight: 700;
}
QPushButton#markPaidBtn:hover,
QPushButton#successBtn:hover,
QPushButton#completeBtn:hover {
    background-color: #059669;
    color: #FFFFFF;
}

/* Neutral — cancel dialog, view receipt */
QPushButton#cancelBtn,
QPushButton#viewRcptBtn,
QPushButton#rcptCloseBtn {
    background-color: #F1F5F9;
    color: #475569;
    border: 1.5px solid #CBD5E0;
    border-radius: 8px;
    padding: 6px 16px;
    font-size: 10pt;
    font-weight: 600;
}
QPushButton#cancelBtn:hover,
QPushButton#viewRcptBtn:hover,
QPushButton#rcptCloseBtn:hover {
    background-color: #E2E8F0;
    color: #1E293B;
}

/* Save receipt */
QPushButton#rcptSaveBtn {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 6px 18px;
    font-size: 10pt;
    font-weight: 700;
}
QPushButton#rcptSaveBtn:hover { background-color: #1D4ED8; }

/* ──────────────────────────────────────────────────────────────
   4. QTABLEWIDGET — alternating rows, dark header
   ────────────────────────────────────────────────────────────── */
QTableWidget {
    background-color: #FFFFFF;
    alternate-background-color: #F8FAFC;
    border: 1px solid #E2E8F0;
    border-radius: 10px;
    gridline-color: transparent;
    font-size: 10pt;
    selection-background-color: #EFF6FF;
    selection-color: #1E40AF;
}

QTableWidget::item {
    padding: 6px 10px;
    border-bottom: 1px solid #F1F5F9;
}

QTableWidget::item:selected {
    background-color: #EFF6FF;
    color: #1E40AF;
}

/* Dark header */
QHeaderView::section {
    background-color: #1A2744;
    color: #E2E8F0;
    font-size: 9pt;
    font-weight: 700;
    letter-spacing: 0.8px;
    padding: 10px 10px;
    border: none;
    border-right: 1px solid rgba(255,255,255,0.08);
    text-transform: uppercase;
}

QHeaderView::section:first {
    border-top-left-radius: 10px;
}

QHeaderView::section:last {
    border-top-right-radius: 10px;
    border-right: none;
}

/* ──────────────────────────────────────────────────────────────
   5. QDIALOG — white card, soft shadow via border
   ────────────────────────────────────────────────────────────── */
QDialog {
    background-color: #FFFFFF;
    border-radius: 14px;
}

/* Dialog internal header bar (used in Billing/Receipt dialogs) */
QFrame#dialogHeader,
QFrame#rcptHeader {
    background-color: #1A2744;
    border-radius: 0px;
}

QLabel#dialogTitle,
QLabel#rcptHeaderTitle {
    color: #FFFFFF;
    font-size: 12pt;
    font-weight: 700;
}

/* Error banner inside dialogs */
QLabel#errorLabel {
    background-color: #FEF2F2;
    color: #B91C1C;
    border: 1px solid #FECACA;
    border-radius: 8px;
    padding: 8px 12px;
    font-size: 9pt;
    font-weight: 600;
}

/* ──────────────────────────────────────────────────────────────
   6. QLINEEDIT & QCOMBOBOX — rounded, focus highlight
   ────────────────────────────────────────────────────────────── */
QLineEdit,
QLineEdit#formControl {
    background-color: #F8FAFC;
    border: 1.5px solid #E2E8F0;
    border-radius: 8px;
    padding: 8px 12px;
    font-size: 10pt;
    color: #1A202C;
    selection-background-color: #BFDBFE;
}

QLineEdit:focus,
QLineEdit#formControl:focus {
    border-color: #2563EB;
    background-color: #FFFFFF;
}

QLineEdit:hover {
    border-color: #94A3B8;
}

QComboBox,
QComboBox#formControl,
QComboBox#filterCombo {
    background-color: #F8FAFC;
    border: 1.5px solid #E2E8F0;
    border-radius: 8px;
    padding: 7px 12px;
    font-size: 10pt;
    color: #1A202C;
    min-height: 20px;
}

QComboBox:focus,
QComboBox#formControl:focus,
QComboBox#filterCombo:focus {
    border-color: #2563EB;
    background-color: #FFFFFF;
}

QComboBox:hover {
    border-color: #94A3B8;
}

QComboBox::drop-down {
    border: none;
    width: 28px;
    subcontrol-origin: padding;
    subcontrol-position: right center;
}

QComboBox::down-arrow {
    width: 12px;
    height: 12px;
}

QComboBox QAbstractItemView {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    selection-background-color: #EFF6FF;
    selection-color: #1E40AF;
    padding: 4px;
}

/* SpinBox — fee inputs in billing dialog */
QDoubleSpinBox,
QDoubleSpinBox#formControl,
QSpinBox,
QSpinBox#ageSpinBox {
    background-color: #F8FAFC;
    border: 1.5px solid #E2E8F0;
    border-radius: 8px;
    padding: 7px 10px;
    font-size: 10pt;
    color: #1A202C;
}

QDoubleSpinBox:focus,
QDoubleSpinBox#formControl:focus,
QSpinBox:focus,
QSpinBox#ageSpinBox:focus {
    border-color: #2563EB;
    background-color: #FFFFFF;
}

QDoubleSpinBox::up-button, QDoubleSpinBox::down-button,
QSpinBox::up-button,       QSpinBox::down-button {
    width: 20px;
    border: none;
    background-color: #EDF2F7;
    border-radius: 4px;
}

/* DateEdit */
QDateEdit,
QDateEdit#formControl,
QDateEdit#admissionDate {
    background-color: #F8FAFC;
    border: 1.5px solid #E2E8F0;
    border-radius: 8px;
    padding: 7px 10px;
    font-size: 10pt;
    color: #1A202C;
}

QDateEdit:focus,
QDateEdit#formControl:focus,
QDateEdit#admissionDate:focus {
    border-color: #2563EB;
    background-color: #FFFFFF;
}

QDateEdit::drop-down {
    border: none;
    width: 28px;
    background: transparent;
}

/* ──────────────────────────────────────────────────────────────
   7. DASHBOARD STAT CARDS — rounded corners, white bg, shadow
      These rules apply to any QFrame with objectName "statCard"
      e.g.  frame->setObjectName("statCard");
   ────────────────────────────────────────────────────────────── */
QFrame#statCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
    padding: 18px;
}

QFrame#statCard:hover {
    border-color: #93C5FD;
    background-color: #F8FAFF;
}

QLabel#statValue {
    font-size: 26pt;
    font-weight: 800;
    color: #1A202C;
}

QLabel#statTitle {
    font-size: 9pt;
    font-weight: 600;
    color: #718096;
    letter-spacing: 0.5px;
    text-transform: uppercase;
}

QLabel#statChange {
    font-size: 9pt;
    font-weight: 600;
    color: #059669;
}

/* ──────────────────────────────────────────────────────────────
   MISC — scrollbars, content area, billing summary bar
   ────────────────────────────────────────────────────────────── */
QScrollBar:vertical {
    background-color: #F0F4F8;
    width: 8px;
    margin: 0;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background-color: #CBD5E0;
    border-radius: 4px;
    min-height: 24px;
}
QScrollBar::handle:vertical:hover {
    background-color: #94A3B8;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical { height: 0; }

QScrollBar:horizontal {
    background-color: #F0F4F8;
    height: 8px;
    margin: 0;
    border-radius: 4px;
}
QScrollBar::handle:horizontal {
    background-color: #CBD5E0;
    border-radius: 4px;
    min-width: 24px;
}
QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal { width: 0; }

QStackedWidget#contentArea {
    background-color: #F0F4F8;
}

QWidget#page {
    background-color: #F0F4F8;
}

/* Billing summary strip */
QFrame#summaryStrip {
    background-color: #EFF6FF;
    border-bottom: 1px solid #BFDBFE;
}

QLabel#summaryLabel {
    font-size: 9pt;
    color: #1A202C;
}

/* Total amount banner in billing dialog */
QFrame#totalBar {
    background-color: #EFF6FF;
    border-top: 2px solid #2563EB;
}

QLabel#totalTag {
    font-size: 8pt;
    font-weight: 700;
    color: #64748B;
    letter-spacing: 1.5px;
}

QLabel#totalValue {
    font-size: 16pt;
    font-weight: 800;
    color: #1A2744;
}

/* Receipt text view */
QTextEdit#receiptView {
    background-color: #FFFFFF;
    border: none;
    font-family: "Courier New", monospace;
    font-size: 10pt;
}

/* Page subtitle on placeholder pages */
QLabel#pageSubLabel {
    color: #718096;
    font-size: 11pt;
}

    )");

    MainWindow w;

    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width()  - w.width())  / 2;
    int y = (screenGeometry.height() - w.height()) / 2;
    w.move(x, y);

    w.show();
    return app.exec();
}
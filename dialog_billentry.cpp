#include "dialog_billentry.h"
#include "ui_dialog_billentry.h"

DialogBillEntry::DialogBillEntry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBillEntry)
{
    ui->setupUi(this);

    ui->dateEdit_occurance->setDateTime(QDateTime::currentDateTime());

    connect(this, SIGNAL(accepted()), this, SLOT(on_accepted()));
}

DialogBillEntry::~DialogBillEntry()
{
    delete ui;
}

void DialogBillEntry::on_accepted()
{
    m_result.name = ui->lineEdit->text();
    m_result.interval = ui->spinBox_interval->value();
    m_result.amount = ui->doubleSpinBox_amount->value();
    m_result.last_occurance = ui->dateEdit_occurance->dateTime();
}

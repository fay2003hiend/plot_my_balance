#include "dialog_billentry.h"
#include "ui_dialog_billentry.h"

DialogBillEntry::DialogBillEntry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBillEntry)
{
    ui->setupUi(this);
}

DialogBillEntry::~DialogBillEntry()
{
    delete ui;
}

#include "widgetmain.h"
#include "./ui_widgetmain.h"
#include "dialog_billentry.h"

WidgetMain::WidgetMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetMain)
{
    ui->setupUi(this);
}

WidgetMain::~WidgetMain()
{
    delete ui;
}

void WidgetMain::on_btn_add_pressed()
{
    DialogBillEntry *dlg = new DialogBillEntry(this);
    dlg->exec();
}

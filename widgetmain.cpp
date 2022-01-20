#include "widgetmain.h"
#include "./ui_widgetmain.h"
#include "dialog_billentry.h"

BillEntryModel::BillEntryModel(QObject *parent)
    : QStandardItemModel(parent)
{
    QStringList labels = {
        tr("Item"),
        tr("Interval"),
        tr("Amount"),
        tr("Last occurance")
    };
    setHorizontalHeaderLabels(labels);
}

BillEntryModel::~BillEntryModel()
{

}

void BillEntryModel::addEntry(const BillEntry &entry)
{
    auto idx = rowCount();
    insertRow(idx);

    setData(index(idx, 0), entry.uuid, Qt::UserRole);
    setData(index(idx, 0), Qt::Unchecked, Qt::CheckStateRole);
    item(idx, 0)->setCheckable(true);

    setData(index(idx, 0), entry.name);
    setData(index(idx, 1), entry.interval);
    setData(index(idx, 2), entry.amount);
    setData(index(idx, 3), entry.last_occurance.toSecsSinceEpoch(), Qt::UserRole);
    setData(index(idx, 3), entry.last_occurance.toString("yyyy/MM/dd"));
}

PlotEntryModel::PlotEntryModel(QObject *parent)
    : QStandardItemModel(parent)
{
    QStringList labels = {
        tr("Date"),
        tr("Item"),
        tr("Amount"),
        tr("Balance")
    };
    setHorizontalHeaderLabels(labels);
}

PlotEntryModel::~PlotEntryModel()
{

}

WidgetMain::WidgetMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetMain)
{
    ui->setupUi(this);

    m_bill_model = new BillEntryModel(this);
    ui->treeView_bills->setModel(m_bill_model);

    m_plot_model = new PlotEntryModel(this);
    ui->treeView_plot->setModel(m_plot_model);

    ui->dateEdit_begin->setDateTime(QDateTime::currentDateTime());
}

WidgetMain::~WidgetMain()
{
    delete ui;
}

void WidgetMain::on_btn_add_pressed()
{
    DialogBillEntry *dlg = new DialogBillEntry(this);
    if (QDialog::Accepted == dlg->exec())
    {
        m_bill_model->addEntry(dlg->get_result());
    }
}

void WidgetMain::on_btn_remove_pressed()
{
    for(int i=0; i<m_bill_model->rowCount(); i++) {
        auto it = m_bill_model->item(i, 0);
        if (it->data(Qt::CheckStateRole) == Qt::Checked) {
            m_bill_model->removeRow(i);
            --i;
        }
    }
}

void WidgetMain::on_btn_plot_pressed()
{
    QList<BillEntry> bills;

    for(int i=0; i<m_bill_model->rowCount(); i++) {
        auto it = m_bill_model->item(i, 0);

        BillEntry e;
        e.uuid = it->data(Qt::UserRole).toString();
        e.name = it->data(Qt::EditRole).toString();
        e.interval = m_bill_model->item(i, 1)->data(Qt::EditRole).toInt();
        e.amount = m_bill_model->item(i, 2)->data(Qt::EditRole).toDouble();
        auto secs2epoch = m_bill_model->item(i, 3)->data(Qt::UserRole).toLongLong();
        e.last_occurance = QDateTime::fromSecsSinceEpoch(secs2epoch);

        bills.push_back(e);
    }

    m_plot_model->removeRows(0, m_plot_model->rowCount());
    if (bills.empty()) return;

    auto begin_date = ui->dateEdit_begin->dateTime();
    auto end_date = begin_date.addYears(2);

    QList<BillEntry> execution_list;
    foreach(const auto& bill, bills) {
        // find first occurance that is within our plot
        auto d = bill.last_occurance;
        while (d < begin_date) {
            d = d.addDays(bill.interval);
        }

        while (d < end_date) {
            BillEntry exec;
            exec.name = bill.name;
            exec.amount = bill.amount;
            exec.last_occurance = d;
            execution_list.push_back(exec);
            d = d.addDays(bill.interval);
        }
    }

    std::sort(execution_list.begin(), execution_list.end(),
          [](const BillEntry& lhs, const BillEntry& rhs) {
        return lhs.last_occurance < rhs.last_occurance;
    });

    int idx = 0;
    double balance = ui->doubleSpinBox_balance->value();
    foreach(const auto&entry, execution_list)
    {
        m_plot_model->insertRow(idx);

        balance += entry.amount;
        m_plot_model->setData(m_plot_model->index(idx, 0), entry.last_occurance.toString("yyyy/MM/dd"));
        m_plot_model->setData(m_plot_model->index(idx, 1), entry.name);
        m_plot_model->setData(m_plot_model->index(idx, 2), entry.amount);
        m_plot_model->setData(m_plot_model->index(idx, 3), balance);

        ++idx;
    }
}

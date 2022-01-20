#include "widgetmain.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

#include "./ui_widgetmain.h"
#include "dialog_billentry.h"

BillEntryModel::BillEntryModel(QObject *parent) : QStandardItemModel(parent)
{
    QStringList labels = {tr("Item"), tr("Interval"), tr("Amount"), tr("Last occurance"), tr("Est. annual amount")};
    setHorizontalHeaderLabels(labels);
}

BillEntryModel::~BillEntryModel() {}

void BillEntryModel::addEntry(const BillEntry &entry)
{
    auto idx = rowCount();
    insertRow(idx);

    setData(index(idx, 0), entry.uuid, Qt::UserRole);
    setData(index(idx, 0), Qt::Unchecked, Qt::CheckStateRole);
    item(idx, 0)->setCheckable(true);

    updateEntry(idx, entry);
}

void BillEntryModel::updateEntry(int row, const BillEntry &entry)
{
    setData(index(row, 0), entry.name);
    setData(index(row, 1), entry.interval);
    setData(index(row, 2), entry.amount);
    setData(index(row, 3), entry.last_occurance.toSecsSinceEpoch(), Qt::UserRole);
    setData(index(row, 3), entry.last_occurance.toString("yyyy/MM/dd"));

    double num_occurance = 365.25 / entry.interval;
    double est_annual_amount = num_occurance * entry.amount;
    setData(index(row, 4), est_annual_amount);
}

BillEntry BillEntryModel::getEntry(int row) const
{
    BillEntry e;
    auto it = item(row, 0);
    e.uuid = it->data(Qt::UserRole).toString();
    e.name = it->data(Qt::EditRole).toString();
    e.interval = item(row, 1)->data(Qt::EditRole).toInt();
    e.amount = item(row, 2)->data(Qt::EditRole).toDouble();
    auto secs2epoch = item(row, 3)->data(Qt::UserRole).toLongLong();
    e.last_occurance = QDateTime::fromSecsSinceEpoch(secs2epoch);
    return e;
}

PlotEntryModel::PlotEntryModel(QObject *parent) : QStandardItemModel(parent)
{
    QStringList labels = {tr("Date"), tr("Item"), tr("Amount"), tr("Balance")};
    setHorizontalHeaderLabels(labels);
}

PlotEntryModel::~PlotEntryModel() {}

void PlotEntryModel::addEntry(const BillEntry &entry, double balance)
{
    auto idx = rowCount();
    insertRow(idx);

    setData(index(idx, 0), entry.last_occurance.toSecsSinceEpoch(), Qt::UserRole);
    setData(index(idx, 0), entry.last_occurance.toString("yyyy/MM/dd"));
    setData(index(idx, 1), entry.name);
    setData(index(idx, 2), entry.amount);
    setData(index(idx, 3), balance);
}

BillEntry PlotEntryModel::getEntry(int row) const
{
    BillEntry e;

    auto secs2epoch = item(row, 0)->data(Qt::UserRole).toLongLong();
    e.last_occurance = QDateTime::fromSecsSinceEpoch(secs2epoch);
    e.name = item(row, 1)->data(Qt::EditRole).toString();
    e.amount = item(row, 2)->data(Qt::EditRole).toDouble();
    e.balance = item(row, 3)->data(Qt::EditRole).toDouble();

    return e;
}

WidgetMain::WidgetMain(QWidget *parent) : QWidget(parent), ui(new Ui::WidgetMain)
{
    ui->setupUi(this);

    m_bill_model = new BillEntryModel(this);
    ui->treeView_bills->setModel(m_bill_model);

    m_plot_model = new PlotEntryModel(this);
    ui->treeView_plot->setModel(m_plot_model);

    ui->dateEdit_begin->setDateTime(QDateTime::currentDateTime());

    connect(ui->treeView_bills, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_billentry_double_clicked(QModelIndex)));
}

WidgetMain::~WidgetMain() { delete ui; }

void WidgetMain::on_btn_add_pressed()
{
    DialogBillEntry *dlg = new DialogBillEntry(this);
    if (QDialog::Accepted == dlg->exec()) {
        m_bill_model->addEntry(dlg->get_result());
    }
}

void WidgetMain::on_btn_remove_pressed()
{
    for (int i = 0; i < m_bill_model->rowCount(); i++) {
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

    for (int i = 0; i < m_bill_model->rowCount(); i++) {
        BillEntry e = m_bill_model->getEntry(i);
        bills.push_back(e);
    }

    m_plot_model->removeRows(0, m_plot_model->rowCount());
    if (bills.empty()) return;

    auto begin_date = ui->dateEdit_begin->dateTime();
    auto end_date = begin_date.addYears(2);

    QList<BillEntry> execution_list;
    foreach (const auto &bill, bills) {
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
              [](const BillEntry &lhs, const BillEntry &rhs) { return lhs.last_occurance < rhs.last_occurance; });

    double balance = ui->doubleSpinBox_balance->value();
    foreach (const auto &entry, execution_list) {
        balance += entry.amount;
        m_plot_model->addEntry(entry, balance);
    }
}

void WidgetMain::slot_billentry_double_clicked(QModelIndex idx)
{
    auto row = idx.row();
    auto e = m_bill_model->getEntry(row);
    DialogBillEntry *dlg = new DialogBillEntry(this);
    dlg->setupContents(e);
    if (QDialog::Accepted == dlg->exec()) {
        m_bill_model->updateEntry(row, dlg->get_result());
    }
}

static QStringList parse_csv_fields(const QByteArray &line)
{
    bool inside_quote = false;
    QString cur;
    QStringList ret;

    for (auto it = line.begin(); it != line.end(); ++it) {
        auto ch = *it;
        switch (ch) {
        case '"':
        case '\'':
            if (cur.isEmpty()) {
                // beginning of a string field?
                inside_quote = true;
            } else if (inside_quote) {
                inside_quote = false;
            }
            break;
        case ',':
            if (inside_quote) {
                cur.push_back(ch);
            } else {
                ret.push_back(cur);
                cur.clear();
                inside_quote = false;
            }
            break;
        default:
            cur.push_back(ch);
            break;
        }
    }

    // last field
    if (!cur.isEmpty()) {
        ret.push_back(cur);
    }
    return ret;
}

void WidgetMain::on_btn_import_pressed()
{
    static QString last_path;
    auto file_path = QFileDialog::getOpenFileName(this, tr("CSV file to import"), last_path, "*.csv");
    if (file_path.isEmpty()) {
        return;
    }
    last_path = QFileInfo(file_path).dir().canonicalPath();

    QFile fr(file_path);
    if (!fr.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("could not read this file!"));
        return;
    }

    auto ba = fr.readAll();
    fr.close();

    auto lines = ba.split('\n');
    for (auto &line : lines) {
        while (line.endsWith('\r')) {
            line.chop(1);
        }
        line = line.simplified();
        if (line.isEmpty()) continue;
        if (line.startsWith("Item,Interval days,Amount,Last Occurance")) continue;

        QStringList fields = parse_csv_fields(line);

        if (fields.count() < 4) {
            continue;
        }

        BillEntry e;
        e.name = fields[0];
        e.interval = fields[1].toInt();
        e.amount = fields[2].toDouble();
        e.last_occurance = QDateTime::fromString(fields[3], "yyyy/MM/dd");
        m_bill_model->addEntry(e);
    }
}

void WidgetMain::on_btn_export_pressed()
{
    static QString last_path;
    if (m_bill_model->rowCount() < 1) {
        return;
    }

    auto file_path = QFileDialog::getSaveFileName(this, tr("CSV file to export"), last_path, "*.csv");
    if (file_path.isEmpty()) {
        return;
    }
    last_path = QFileInfo(file_path).dir().canonicalPath();

    QFile fw(file_path);
    if (!fw.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("could not write to this file!"));
        return;
    }

    fw.write("Item,Interval days,Amount,Last Occurance\r\n");
    for (int i = 0; i < m_bill_model->rowCount(); i++) {
        auto e = m_bill_model->getEntry(i);
        QStringList list;
        list.push_back(QString("\"%1\"").arg(e.name));
        list.push_back(QString::number(e.interval));
        list.push_back(QString::asprintf("%f", e.amount));
        list.push_back(e.last_occurance.toString("yyyy/MM/dd"));
        fw.write(list.join(',').toLocal8Bit());
        fw.write("\r\n");
    }
    fw.close();
}

void WidgetMain::on_btn_export_plot_pressed()
{
    static QString last_path;

    // press plot if there is no item
    // if still no item after plot, we return
    if (m_plot_model->rowCount() < 1) {
        on_btn_plot_pressed();
    }
    if (m_plot_model->rowCount() < 1) {
        return;
    }

    auto file_path = QFileDialog::getSaveFileName(this, tr("CSV file to export"), last_path, "*.csv");
    if (file_path.isEmpty()) {
        return;
    }
    last_path = QFileInfo(file_path).dir().canonicalPath();

    QFile fw(file_path);
    if (!fw.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("could not write to this file!"));
        return;
    }

    fw.write("Date,Item,Amount,Balance\r\n");
    for (int i = 0; i < m_plot_model->rowCount(); i++) {
        auto e = m_plot_model->getEntry(i);
        QStringList list;
        list.push_back(e.last_occurance.toString("yyyy/MM/dd"));
        list.push_back(QString("\"%1\"").arg(e.name));
        list.push_back(QString::asprintf("%f", e.amount));
        list.push_back(QString::asprintf("%f", e.balance));
        fw.write(list.join(',').toLocal8Bit());
        fw.write("\r\n");
    }
    fw.close();
}

#ifndef DIALOG_BILLENTRY_H
#define DIALOG_BILLENTRY_H

#include <QDateTime>
#include <QDialog>
#include <QUuid>

namespace Ui
{
class DialogBillEntry;
}

struct BillEntry {
    QString uuid;
    QString name;
    int interval;
    double amount;
    QDateTime last_occurance;

    double balance = 0.0;

    BillEntry() { uuid = QUuid::createUuid().toString(); }
};

class DialogBillEntry : public QDialog
{
    Q_OBJECT

   public:
    explicit DialogBillEntry(QWidget *parent = nullptr);
    ~DialogBillEntry();

    BillEntry get_result() { return m_result; }
    void setupContents(const BillEntry &info);

   private slots:
    void on_accepted();

   private:
    BillEntry m_result;

    Ui::DialogBillEntry *ui;
};

#endif  // DIALOG_BILLENTRY_H

#ifndef DIALOG_BILLENTRY_H
#define DIALOG_BILLENTRY_H

#include <QDialog>

namespace Ui {
class DialogBillEntry;
}

class DialogBillEntry : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBillEntry(QWidget *parent = nullptr);
    ~DialogBillEntry();

private:
    Ui::DialogBillEntry *ui;
};

#endif // DIALOG_BILLENTRY_H

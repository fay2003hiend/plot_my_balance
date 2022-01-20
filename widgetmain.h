#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include <QStandardItemModel>
#include <QWidget>

#include "dialog_billentry.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class WidgetMain;
}
QT_END_NAMESPACE

class BillEntryModel : public QStandardItemModel
{
   public:
    BillEntryModel(QObject *parent);
    ~BillEntryModel();

    void addEntry(const BillEntry &entry);
    void updateEntry(int row, const BillEntry &entry);
    BillEntry getEntry(int row) const;
};

class PlotEntryModel : public QStandardItemModel
{
   public:
    PlotEntryModel(QObject *parent);
    ~PlotEntryModel();

    void addEntry(const BillEntry &entry, double balance);
    BillEntry getEntry(int row) const;
};

class WidgetMain : public QWidget
{
    Q_OBJECT

   public:
    WidgetMain(QWidget *parent = nullptr);
    ~WidgetMain();

   private slots:
    void on_btn_add_pressed();
    void on_btn_remove_pressed();
    void on_btn_plot_pressed();

    void slot_billentry_double_clicked(QModelIndex);

    void on_btn_import_pressed();
    void on_btn_export_pressed();
    void on_btn_export_plot_pressed();

   private:
    BillEntryModel *m_bill_model;
    PlotEntryModel *m_plot_model;

    Ui::WidgetMain *ui;
};
#endif  // WIDGETMAIN_H

#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include <QWidget>
#include <QStandardItemModel>
#include "dialog_billentry.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetMain; }
QT_END_NAMESPACE

class BillEntryModel : public QStandardItemModel
{
public:
    BillEntryModel(QObject *parent);
    ~BillEntryModel();

    void addEntry(const BillEntry& entry);
};

class PlotEntryModel : public QStandardItemModel
{
public:
    PlotEntryModel(QObject *parent);
    ~PlotEntryModel();
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

private:
    BillEntryModel *m_bill_model;
    PlotEntryModel *m_plot_model;

    Ui::WidgetMain *ui;
};
#endif // WIDGETMAIN_H

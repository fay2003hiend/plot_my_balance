#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetMain; }
QT_END_NAMESPACE

class WidgetMain : public QWidget
{
    Q_OBJECT

public:
    WidgetMain(QWidget *parent = nullptr);
    ~WidgetMain();

private slots:
    void on_btn_add_pressed();

private:
    Ui::WidgetMain *ui;
};
#endif // WIDGETMAIN_H

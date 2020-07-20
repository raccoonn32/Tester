#ifndef CONFIRMW_H
#define CONFIRMW_H

#include <QWidget>
#include <QEventLoop>
#include <QCloseEvent>
#include <QDialog>

#include <QTimer>
#include "task.h"

namespace Ui {
class ConfirmW;
}

class ConfirmW : public QWidget
{
    Q_OBJECT

public:
    explicit ConfirmW(QWidget *parent = 0);
    ~ConfirmW();
    int exec();
    void setText(const QString &s);
    void setGPIOButton(int pin1,int pin2);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void onOkClicked(bool v);
    void onCancelClicked(bool v);
    void onGpioInput();
private:
    Ui::ConfirmW *ui;
    QEventLoop *m_eventLoop;
    int m_chooseResult;
    GpioReadTask *m_gpio_read;
    int pinOk;
    int pinCancel;
    gpio m_gpiopin;
    QTimer *m_timer;
};

#endif // CONFIRMW_H

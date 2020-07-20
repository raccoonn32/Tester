#ifndef SELFTESTDIALOG_H
#define SELFTESTDIALOG_H

#include <QDialog>

#include "globalcommon.h"
#include "modbus_backend.h"

#include <QTimer>
#include <QEventLoop>

namespace Ui {
class SelfTestDialog;
}

class SelfTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelfTestDialog(QWidget *parent = 0);
    ~SelfTestDialog();
    void closeEvent(QCloseEvent *event);
    void startTest();

signals:
    void selftestover();

private slots:
    void on_pb_cancel_clicked();

    void onTimeTick();

private:
    void selfTestStart();
    void selfTestStep1();
private:
    Ui::SelfTestDialog *ui;
    modbus_backend *m_mb232;
    QTimer *m_timerout;
    int m_remaintime; //剩余时间
};

#endif // SELFTESTDIALOG_H

#ifndef DEBUGLOGFORM_H
#define DEBUGLOGFORM_H

#include <QWidget>

namespace Ui {
class DebugLogForm;
}

class DebugLogForm : public QWidget
{
    Q_OBJECT

public:
    explicit DebugLogForm(QWidget *parent = 0);
    ~DebugLogForm();

#if QT_VERSION < 0x050000
    void outputMessage(QtMsgType type,const char* msg);
#else
    void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#endif

public slots:
    void addLog(QString msg);


private slots:
    void on_pushButton_clicked();

private:
    Ui::DebugLogForm *ui;
};

#endif // DEBUGLOGFORM_H

#include "debuglogform.h"
#include "ui_debuglogform.h"
#include <QMutex>
#include <QDateTime>

DebugLogForm::DebugLogForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugLogForm)
{
    ui->setupUi(this);
}

DebugLogForm::~DebugLogForm()
{
    delete ui;
}

void DebugLogForm::addLog(QString msg)
{
    ui->textBrowser->append(msg);
}

void DebugLogForm::on_pushButton_clicked()
{
    ui->textBrowser->clear();
}

#if QT_VERSION < 0x050000
void DebugLogForm::outputMessage(QtMsgType type,const char* msg)
#else
void DebugLogForm::outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#endif
{
    // 加锁
    static QMutex mutex;
    mutex.lock();
#if QT_VERSION < 0x050000
    QString localqMsg(msg);
    QByteArray localMsg = localqMsg.toAscii();
#else
    QByteArray localMsg = msg.toLocal8Bit();
#endif
    QString strMsg("");
    switch((int)type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        abort();
        break;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("hh:mm:ss");
#if QT_VERSION < 0x050000
    QString strMessage = QString("%1 %2 DTime:%3")
            .arg(strMsg).arg(localMsg.constData()).arg(strDateTime);
#else
    QString strMessage = QString("%1 %2 File:%3  Line:%4  Func:%5  DTime:%6")
            .arg(strMsg).arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(strDateTime);
#endif
addLog(strMessage);
mutex.unlock();
}

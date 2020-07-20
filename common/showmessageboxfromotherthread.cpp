#include "showmessageboxfromotherthread.h"

#include <QDebug>


int ShowMessageBoxFromOtherThread::m_result = 0;

ShowMessageBoxFromOtherThread::ShowMessageBoxFromOtherThread(const QString &title, const QString &message, const int kind):
    m_title(title),
    m_message(message),
    m_kind(kind)
{ }

int ShowMessageBoxFromOtherThread::show(const QString &title, const QString &message, const int kind)
{
    QEventLoop eventLoop;
    ShowMessageBoxFromOtherThread *messageBox = new ShowMessageBoxFromOtherThread(title, message,kind);
    connect(messageBox, SIGNAL(destroyed()), &eventLoop, SLOT(quit()));
    messageBox->readyShow();
    eventLoop.exec();
    return m_result;
}

void ShowMessageBoxFromOtherThread::readyShow(void)
{
    this->moveToThread(qApp->thread());
    QTimer::singleShot(0, this, SLOT(onShow()));
}

void ShowMessageBoxFromOtherThread::onShow(void)
{
    if(m_kind == QMessageBox::Question)
    {
        m_result = QMessageBox::question(NULL, m_title, m_message,QMessageBox::Yes,QMessageBox::No);
    }else if(m_kind == QMessageBox::Warning)
    {
        m_result = QMessageBox::warning(NULL, m_title, m_message);
    }else if(m_kind == QMessageBox::Critical)
    {
        m_result = QMessageBox::critical(NULL, m_title, m_message);
    }else
    {
        m_result = QMessageBox::information(NULL, m_title, m_message);
    }

    this->deleteLater();
}

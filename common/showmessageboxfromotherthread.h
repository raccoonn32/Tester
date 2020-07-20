#ifndef SHOWMESSAGEBOXFROMOTHERTHREAD_H
#define SHOWMESSAGEBOXFROMOTHERTHREAD_H

#include <QMessageBox>
#include <QEventLoop>
#include <QTimer>
#include <QApplication>

class ShowMessageBoxFromOtherThread : public QObject
{
    Q_OBJECT
public:
    explicit ShowMessageBoxFromOtherThread(const QString &title, const QString &message,const int kind=QMessageBox::Information);

    static int show(const QString &title, const QString &message, const int kind=QMessageBox::Information);

signals:


private:
    const QString m_title;
    const QString m_message;
    const int m_kind;//类型 提示 警告  询问。。。
    static int   m_result;
public slots:


private:
    void readyShow(void);

private slots:
    void onShow(void);

};

#endif // SHOWMESSAGEBOXFROMOTHERTHREAD_H

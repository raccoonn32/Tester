#ifndef RTHREADOBJ_H
#define RTHREADOBJ_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <limits>

#include <QDebug>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QElapsedTimer>
#include <QCoreApplication>
#endif

class rThreadObj : public QObject
{
    Q_OBJECT
public:
    explicit rThreadObj(const QString &info = NULL);
    virtual ~rThreadObj();

    virtual void run();
    void stop();
    void setRunCount(int count);

signals:
    void message(const QString& info);
    void resultReady(const QString &result);
    void operate();//开始Run

public slots:
    void start();

signals:

private slots:
    void m_run();

private:
    int m_runCount;
    bool m_isStop;
    QString m_info;
    QMutex m_stopMutex;
};

#endif // RTHREADOBJ_H

#include "rthreadobj.h"

//parent 为NULL  必须为NULL
rThreadObj::rThreadObj(const QString &info) :
    QObject(NULL)
  ,m_isStop(true)
  ,m_info(info)
  ,m_runCount(1)
{
    //connect(this,&rThreadObj::operate,this,&rThreadObj::m_run);
    connect(this,SIGNAL(operate()),this,SLOT(m_run()));
}

rThreadObj::~rThreadObj()
{
    qDebug() << "ThreadObject destroy";
    emit message(QString("Destroy %1->%2,thread id:%3").arg(__FUNCTION__).arg(__FILE__).arg((int)QThread::currentThreadId()));
}

void rThreadObj::setRunCount(int count)
{
    m_runCount = count;
    emit message(QString("%1->%2,thread id:%3").arg(__FUNCTION__).arg(__FILE__).arg((int)QThread::currentThreadId()));
}

void rThreadObj::stop()
{
    QMutexLocker locker(&m_stopMutex);
    emit message(QString("%1->%2,thread id:%3").arg(__FUNCTION__).arg(__FILE__).arg((int)QThread::currentThreadId()));
    m_isStop = true;
}

void rThreadObj::start()
{
    emit message(QString("%1->%2,thread id:%3").arg(__FUNCTION__).arg(__FILE__).arg((int)QThread::currentThreadId()));
    emit operate();
}

void rThreadObj::run()
{
    qDebug() << QString("%1->%2,thread id:%3").arg(__FUNCTION__).arg(__FILE__).arg((int)QThread::currentThreadId());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QThread::msleep(500);
#else
        QElapsedTimer t;
        t.start();
        while(t.elapsed()<500)
            QCoreApplication::processEvents();
#endif
}

void rThreadObj::m_run()
{
    {
        QMutexLocker locker(&m_stopMutex);
        m_isStop = false;
    }
    int count = 0;
    QString str = QString("%1->%2,thread id:%3").arg(__FILE__).arg(__FUNCTION__).arg((int)QThread::currentThreadId());
    emit message(str);
    while(1)
    {
        {
            QMutexLocker locker(&m_stopMutex);
            if(m_isStop)
                return;
        }
        if(m_runCount == count)
        {
            break;
        }
        run();
        count++;
    }
    emit resultReady("Thread Work Done!");
}

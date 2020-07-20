#ifndef NEWTHREAD_H
#define NEWTHREAD_H

#include <QThread>

class ThreadFromQThread : public QThread
{
    Q_OBJECT
signals:
    void functionSelect(int item);
    void telnetexecute();
public:
    ThreadFromQThread(QObject* par);
    ~ThreadFromQThread();

    void run();

private:
    bool m_stop;
};

#endif // NEWTHREAD_H

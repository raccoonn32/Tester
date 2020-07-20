#ifndef TASK_H
#define TASK_H

#include <QRunnable>
#include <QMutex>
#include <QThreadPool>
#include <QtTest/QTest>
#include <QObject>
#include <QDebug>

#include "gpio.h"

class GpioReadTask : public QObject ,public QRunnable
{
    Q_OBJECT
public:
    GpioReadTask(){}
//    GpioReadTask(int pin1,int pin2)
//    {
//        isStop = false;
//        pin1_dat = false;pin2_dat = false;defaultVal = false;
//        m_pin1 = pin1;
//        m_pin2 = pin2;
//        m_gpio.setGpio(pin1,pin1_dat);
//        m_gpio.setGpio(pin2,pin2_dat);
////        this->setAutoDelete(false);//不自动回收
//    }
//    virtual ~GpioReadTask(){qDebug() << __FUNCTION__;}

    void run()
    {
        m_lock.lock();
        bool run = !isStop;
        m_lock.unlock();
        while(run)
        {
            pin1_dat = m_gpio.getGpio(m_pin1,defaultVal);
            pin2_dat = m_gpio.getGpio(m_pin2,defaultVal);
            //QTest::qSleep(1000);
            qDebug() << __FUNCTION__ << pin1_dat << pin2_dat;
            if(pin1_dat != defaultVal) {emit presspin(m_pin1); isStop = true;}
            else
            if(pin2_dat != defaultVal) {emit presspin(m_pin2); isStop = true;}
        }
    }

//    //返回生效的那个pin
//    int getPin(void)
//    {
//        if(pin1_dat == true)
//            return m_pin2;
//        else
//            return m_pin2;
//    }

//    void stop(void)
//    {
//        m_lock.lock();
//        isStop = true;
//        m_lock.unlock();
//    }


signals:
    void presspin(int pin);

private:
    QMutex m_lock;
    bool isStop;
    gpio m_gpio;
    int m_pin1,m_pin2;
    bool pin1_dat,pin2_dat;
    bool defaultVal;//读取gpio状态的原始值 默认值
};

#endif // TASK_H

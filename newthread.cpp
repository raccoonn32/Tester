#include "newthread.h"
#include <QDebug>
#include "gpio.h"
#include "globalcommon.h"

ThreadFromQThread::ThreadFromQThread(QObject* par) : QThread(par)
{
    m_stop = false;
}

ThreadFromQThread::~ThreadFromQThread()
{

}

void ThreadFromQThread::run()
{

    gpio gpiosel;
    //bool res_sel, res_exe;
    static int keycount = 0;

    while(1)
    {
        while((gpiosel.getGpio(GPIO_SEL, true) && gpiosel.getGpio(GPIO_TEL, true)));
        msleep(200);

        if(!gpiosel.getGpio(GPIO_SEL, true))
        {
            keycount++;

            if(keycount > 5)
                keycount = 1;

            emit functionSelect(keycount);
            //qDebug() << "keycount:" << keycount;
            GlobalCommon::GetInstance().setTelFunItem(keycount);
        }

        if(!gpiosel.getGpio(GPIO_TEL, true))
        {
            emit telnetexecute();
            //qDebug() << "execute key down!";
        }

        if(m_stop) break;
    }
}



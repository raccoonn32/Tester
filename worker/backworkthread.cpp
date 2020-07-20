#include "backworkthread.h"
#include "qapphelper.h"
#include "globalcommon.h"
#include "showmessageboxfromotherthread.h"

#include <QDebug>
#include <QDateTime>

#ifdef TEST_
#define DELAYTIME 1000
#else
#define DELAYTIME 1000
#endif

BackWorkThread::BackWorkThread()
    :rThreadObj("BakcWork")
{
    QSettings settings("./config.conf",QSettings::IniFormat);
    settings.beginGroup("Config");
    restesterconfig = settings.value("restester").toString().split(',');
    docontrollerconfig =  settings.value("docontroller").toString().split(',');
    settings.endGroup();
    qDebug() << "restester:" << restesterconfig ;
    qDebug() << "docontroller: "<< docontrollerconfig;
    {
        QString str = restesterconfig.join(",");
        emit initconfig(1,str);
    }
    {
        QString str = docontrollerconfig.join(",");
        emit initconfig(2,str);
    }
}

BackWorkThread::~BackWorkThread()
{
}

void BackWorkThread::run()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    qDebug() << __FUNCTION__ << m_currentWork;
    emit running();



    if(m_currentWork == CUTOFF)//切断
    {
        cutoff();
    }else if(m_currentWork == PWONSELFTEST)//自检
    {
        powerOnSelfTest();
    }else if(m_currentWork == PRECHSELFTEST)//通道自检 前置命令
    {
        preChannelSelfTest();
    }else if(m_currentWork == CHSELFTEST)//通道自检
    {
        channelSelfTest(0);
    }else if(m_currentWork == CHSELFTESTINS)
    {
        channelSelfTest(1);
    }else if(m_currentWork == BRITEST)//桥路测量
    {
        bridgetest();
    }else if(m_currentWork == INSTEST)
    {
        instest();//绝缘测量
    }else if(m_currentWork == RELTEST)
    {
        reltest();//泄放测量
    }else if(m_currentWork == FUNCTEST)
    {
        functest();
    }
    qDebug() << __PRETTY_FUNCTION__ << "runover";
    emit runover();
    QApplication::restoreOverrideCursor();

}

void BackWorkThread::start(BackWorkThread::eWorkType a)
{
    if(resTester.isNull())
    {
        qDebug() << "new ResTester" << QThread::currentThread() << __FUNCTION__ ;
        QString port = restesterconfig.at(0);
        int baud = restesterconfig.at(1).toInt();
        char pairty = restesterconfig.at(2).toUpper().at(0).toLatin1();
        int databits = restesterconfig.at(3).toInt();
        int stopbits = restesterconfig.at(4).toInt();
#ifdef WIN32
        resTester = new ResTester(port,baud,pairty,databits,stopbits);
#else
        //resTester = new ResTester("/dev/ttyS5");
        resTester = new ResTester(port,baud,pairty,databits,stopbits);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,7, 0))
        connect(this,&BackWorkThread::sendPowerOnSelfTestCMD,resTester,&ResTester::powerOnSelfTestCMD);

#else
        connect(this,SIGNAL(sendPowerOnSelfTestCMD()),resTester,SLOT(powerOnSelfTestCMD()));
#endif
    }
    if(doController.isNull())
    {
        qDebug() << "new DoController" << QThread::currentThread() << __FUNCTION__ ;
        QString dport = docontrollerconfig.at(0);
        int dbaud = docontrollerconfig.at(1).toInt();
        char dpairty = docontrollerconfig.at(2).toUpper().at(0).toLatin1();
        int databits = docontrollerconfig.at(3).toInt();
        int stopbits = docontrollerconfig.at(4).toInt();
#ifdef WIN32
        doController = new DoController(dport,dbaud,dpairty,databits,stopbits);
#else
        //doController = new DoController("/dev/ttyS4");
        doController = new DoController(dport,dbaud,dpairty,databits,stopbits);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
        connect(this,&BackWorkThread::selectChannel,doController,&DoController::selectChannel);
        connect(this,&BackWorkThread::setDoWorkMode,doController,&DoController::setWorkMode);
#else
        connect(this,SIGNAL(selectChannel(int)),doController,SLOT(selectChannel(int)));
        connect(this,SIGNAL(setDoWorkMode(DoController::eChannelType)),doController,SLOT(setWorkMode(DoController::eChannelType)));
#endif
    }
    if(a == UnKown)
        return;

    m_currentWork = a;
    emit operate();//开始作业
}

void BackWorkThread::powerOnSelfTest()
{
    qDebug() << __FUNCTION__;
    if(doController->hasError() && resTester->hasError())//控制器正常
    {
        emit process(PWONSELFTEST,1, true);
    }else//控制器自检失败
    {
        emit process(PWONSELFTEST,1, false);
        return;
    }

    //res 保存自检结果
    bool res = resTester->powerOnSelfTestCMD();//发送自检命令
    emit process(PWONSELFTEST,2,res);
}


void BackWorkThread::preChannelSelfTest()
{
    qDebug() << __FUNCTION__ << "发送通道自检命令";
    resTester->setPreChanelTestMode();//发送通道自检命令
}

//通道自检
void BackWorkThread::channelSelfTest(int type)
{
#ifdef TEST_
    qsrand(QDateTime::currentMSecsSinceEpoch());
#else
    //设置工作模式，并检测是否设置成功
    {
        int count = 0;//次数
        qint8 res = 0;
        do
        {
            res = resTester->setTestMode();//设置通道出于测量状态
            count++;
            if(res == 7)
            {
                break;
            }
        }while(count<3);

        qDebug() <<__FUNCTION__ << res;
        if(res == 7 && count <3)
        {
            //设置成功
        }else
        {
            //检测仪故障
            ShowMessageBoxFromOtherThread::show("测量失败",QString(tr("检测仪故障,请检查设备后重试!")),QMessageBox::Critical);
            goto the_end;
        }
    }
#endif


    //桥路自检
    if(0 == type){
        qDebug()<< QString::fromLocal8Bit("桥路自检部分开始");
        int totalChannel = TOTALCHANNEL; //默认30路
        int currentChannel = 0;
        doController->setWorkMode(DoController::Bridge);//继电器设置为桥路模式
        //qint8 res = 0;//命令执行结果
        while(totalChannel--)
        {
            currentChannel++;
            emit currentChannelSelfTest(currentChannel);
            qDebug()<< QString::fromLocal8Bit("开始桥路自检，通道: ") << currentChannel <<  QString::fromLocal8Bit("剩余待检测通道数:") << totalChannel;
            //闭合继电器
            doController->selectChannel(currentChannel);
            //选择通道  400101 = 通道号
            resTester->setCurrentChannel(currentChannel);
            //桥路命令  400102 = 7
            bool isCredible = false;
            float resvalue = 0;

            qint8 errorcode = resTester->runTest(CMD_BRI,&isCredible,&resvalue);
            errorcode = resTester->runTest(CMD_BRI,&isCredible,&resvalue);

#ifdef TEST_
            resvalue = 3.1415926 * qrand() /qrand();
            errorcode = -5;
            if( qrand()%3 == 0)
#else
            if(isCredible !=true) //测试结果不可信
#endif
            {
                GlobalCommon::GetInstance().setChannelBridgeValReliable(currentChannel,false);
                emit channelSeflTestRes(0,currentChannel,false,resvalue);

                if(GlobalCommon::GetInstance().getTelFunItem() == 1)
                {
                    if(errorcode == -6) //线路干扰 结束检测
                    {
                        qDebug() << QString::fromLocal8Bit("线路干扰,结束检测,请检查硬件!");
                        goto the_end;
                    }else if(errorcode == -5)
                    {
                        qDebug() << QString(tr("%1号通道测量失败").arg(currentChannel));
                    }
                }
                else
                {
                    if(errorcode == -6) //线路干扰 结束检测
                    {
                        int q = ShowMessageBoxFromOtherThread::show("测量失败",QString(tr("线路干扰,结束检测,请检查硬件!")),QMessageBox::Critical);
                        qDebug() << QString::fromLocal8Bit("线路干扰,结束检测,请检查硬件!");
                        // return;
                        goto the_end;
                    }else if(errorcode == -5)
                    {
                        //n号通道自检失败  询问是否继续
                        int q = ShowMessageBoxFromOtherThread::show("测量失败",QString(tr("%1号通道测量失败,是否继续?").arg(currentChannel)),QMessageBox::Question);
                        qDebug() << QString(tr("%1号通道测量失败,是否继续?").arg(currentChannel));
                        if(q == QMessageBox::No)  //停止
                        {
                            qDebug() << "No";
                            goto the_end;
                        }else{
                            qDebug() << "Yes";
                        }
                    }
                }
            }else
            {
                GlobalCommon::GetInstance().setChannelBridgeSelfTVal(currentChannel,resvalue);
                emit channelSeflTestRes(0,currentChannel,true,resvalue);
            }
            qDebug()<< QString::fromLocal8Bit("桥路自检，通道: " )<< currentChannel  << currentChannel << " value :" << resvalue;
            QAppHelper::delayms(DELAYTIME);//延时1秒
        }

        //emit selfTestOver(type); //通道自检结束
        qDebug() << __FUNCTION__ << "桥路自检 结束";
    }

    //绝缘自检  //按照最新的要求  此处发送的是桥路测量的命令
    if(1 == type){
        qDebug()<< QString::fromLocal8Bit("绝缘自检部分开始");
        int totalChannel = TOTALCHANNEL; //默认30路
        int currentChannel = 0;
        doController->setWorkMode(DoController::Ins);//继电器设置为绝缘模式
        //qint8 res = 0;//命令执行结果
        while(totalChannel--)
        {
            currentChannel++;
            emit currentChannelSelfTest(currentChannel);
            qDebug()<< QString::fromLocal8Bit("开始绝缘自检，通道: ") << currentChannel <<  QString::fromLocal8Bit("剩余待检测通道数:") << totalChannel;
            //闭合继电器
            doController->selectChannel(currentChannel);
            //选择通道  400101 = 通道号
            resTester->setCurrentChannel(currentChannel);
            //绝缘命令  400102 = 15
            bool isCredible = false;
            float resvalue = 0;
            //按照最新的要求  此处发送的是桥路测量的命令  所以不再设置绝缘电压
            qint8 errorcode = resTester->runTest(CMD_INS,&isCredible,&resvalue,0x10);
            errorcode = resTester->runTest(CMD_INS,&isCredible,&resvalue,0x10);

#ifdef TEST_
            resvalue = 3.1415926 * qrand() /qrand();
            errorcode = -5;
            if( qrand()%3 == 0)
#else
            if(isCredible !=true) //测试结果不可信
#endif
            {
                emit channelSeflTestRes(1,currentChannel,false,resvalue);
                GlobalCommon::GetInstance().setChannelInsValReliable(currentChannel,false);

                if(GlobalCommon::GetInstance().getTelFunItem() == 2)
                {
                    if(errorcode == -6) //线路干扰 结束检测
                    {
                        qDebug() << QString::fromLocal8Bit("线路干扰,结束检测,请检查硬件!");
                        goto the_end;
                    }else if(errorcode == -5)
                    {
                        qDebug() << QString(tr("%1号通道测量失败,是否继续?").arg(currentChannel));
                    }
                }
                else
                {
                    if(errorcode == -6) //线路干扰 结束检测
                    {
                        ShowMessageBoxFromOtherThread::show("测量失败",QString(tr("线路干扰,结束检测,请检查硬件!")),QMessageBox::Critical);
                        qDebug() << QString::fromLocal8Bit("线路干扰,结束检测,请检查硬件!");
                        goto the_end;
                    }else if(errorcode == -5)
                    {
                        //n号通道自检失败  询问是否继续
                        int q = ShowMessageBoxFromOtherThread::show("测量失败",QString(tr("%1号通道测量失败,是否继续?").arg(currentChannel)),QMessageBox::Question);
                        qDebug() << QString(tr("%1号通道测量失败,是否继续?").arg(currentChannel));
                        if(q == QMessageBox::No)  //停止
                        {
                            qDebug() << "No";
                            goto the_end;
                        }else{
                            qDebug() << "Yes";
                        }
                    }
                }
            }else
            {
                GlobalCommon::GetInstance().setChannelInsSelfTVal(currentChannel,resvalue);
                emit channelSeflTestRes(1,currentChannel,true,resvalue);
            }

            qDebug()<< QString::fromLocal8Bit("绝缘自检，通道: " )<< currentChannel << " value :" << resvalue;
            QAppHelper::delayms(DELAYTIME);//延时1秒
        }
        //selfTestOver(type);
        qDebug() << __FUNCTION__ << QString::fromLocal8Bit("绝缘自检 结束");
    }

    //cutoff();

the_end  :
    emit selfTestOver(type); //通道自检结束
    qDebug() << __FUNCTION__ << "run end";
}


//桥路测量模式
void BackWorkThread::bridgetest()
{
//#if !defined(Q_OS_WIN32)
//    gpio gpio_brg;
//    gpio_brg.setGpio(GPIO_BRI,true,true); //gpio1_18  输出1
//#endif

    doController->setWorkMode(DoController::Bridge);
    //todo 从GlobalCommon中获取要检测的通道数量 和编号
    //todo 加锁

//#ifdef TEST_
//    QVector<int> channelList = GlobalCommon::GetInstance().selectedChannelList();
//#else
//    QVector<int> channelList;
//    channelList.append(0);
//    channelList.append(1);
//    channelList.append(2);
//    channelList.append(3);
//#endif

    QVector<int> channelList = GlobalCommon::GetInstance().selectedChannelList();
    qDebug()  << __FUNCTION__<< QString::fromLocal8Bit("要测量的通道号列表:")<< channelList ;


    int currentChannel = 0;
    int totalChannel = channelList.size();
    for(int i=0; i<channelList.size(); i++)
    {
        //读入通道编号
        currentChannel = channelList.at(i)+1;
        //设置通道出于测量状态

        emit currentBridgeTest(currentChannel);
        qDebug()<< QString::fromLocal8Bit("开始桥路测试，通道: ") << currentChannel <<  QString::fromLocal8Bit("剩余待检测通道数:") << totalChannel;
        //闭合继电器
        doController->selectChannel(currentChannel);
        //选择通道  400101 = 通道号
        resTester->setCurrentChannel(currentChannel);
        //桥路命令  400102 = 7
        bool isCredible = false;
        float resvalue = 0;

        qint8 errorcode = resTester->runTest(CMD_BRI,&isCredible,&resvalue);
        errorcode = resTester->runTest(CMD_BRI,&isCredible,&resvalue);

        //减去线阻值
        GlobalCommon::GetInstance().getSelfTestRValue();
        //resvalue -= GlobalCommon::GetInstance().bridgeSelfTVal(currentChannel);


        if(isCredible !=true) //测试结果不可信
        {
            GlobalCommon::GetInstance().setChannelBridgeVal(currentChannel,resvalue);
            GlobalCommon::GetInstance().setChannelBridgeValReliable(currentChannel,false);
            emit channelBridgeTestRes(currentChannel,false,resvalue);

            if(GlobalCommon::GetInstance().getTelFunItem() == 3)
            {
                if(errorcode == -6) //线路干扰 结束检测
                {
                    qDebug() << QString::fromLocal8Bit("线路干扰,结束检测,请检查硬件!");
                    goto the_end;
                }else if(errorcode == -5)
                {
                    //n号桥路测试，通道  询问是否继续
                    qDebug() << QString(tr("%1号通道测量失败").arg(currentChannel));
                }
            }
            else
            {
                if(errorcode == -6) //线路干扰 结束检测
                {
                    int q = ShowMessageBoxFromOtherThread::show("测量失败",QString(tr("线路干扰,结束检测,请检查硬件!")),QMessageBox::Critical);
                    qDebug() << QString::fromLocal8Bit("线路干扰,结束检测,请检查硬件!");
                    goto the_end;
                }else if(errorcode == -5)
                {
                    //n号桥路测试，通道  询问是否继续
                    int q = ShowMessageBoxFromOtherThread::show("测量失败",QString(tr("%1号通道测量失败,是否继续?").arg(currentChannel)),QMessageBox::Question);
                    qDebug() << QString(tr("%1号通道测量失败,是否继续?").arg(currentChannel));
                    if(q == QMessageBox::No)  //停止
                    {
                        qDebug() << "No";
                        goto the_end;
                    }else{
                        qDebug() << "Yes";
                    }
                }
            }
        }else
        {
            GlobalCommon::GetInstance().setChannelBridgeValReliable(currentChannel,true);
            GlobalCommon::GetInstance().setChannelBridgeVal(currentChannel,resvalue);
            emit channelBridgeTestRes(currentChannel,true,resvalue);
        }
        qDebug()<< QString::fromLocal8Bit("桥路测试，通道: ") << currentChannel  << currentChannel << " value :" << resvalue;
        QAppHelper::delayms(DELAYTIME);//延时1秒
    }
    //emit bridgeTestOver();

the_end:
    emit bridgeTestOver();
    qDebug() << __FUNCTION__ << "桥路测试 结束";
}

void BackWorkThread::instest()
{
//#if !defined(Q_OS_WIN32)
//    gpio gpio_;
//    gpio_.setGpio(GPIO_INS,true,true); //gpio7_7  输出1
//#endif

    doController->setWorkMode(DoController::Ins);
    //todo 从GlobalCommon中获取要检测的通道数量 和编号
    //todo 加锁
//#ifdef TEST_
//    QVector<int> channelList = GlobalCommon::GetInstance().selectedChannelList();
//#else
//    QVector<int> channelList;
//    channelList.append(0);
//    channelList.append(1);
//    channelList.append(2);
//    channelList.append(3);
//#endif

    QVector<int> channelList = GlobalCommon::GetInstance().selectedChannelList();
    qDebug()  << __FUNCTION__<<QString::fromLocal8Bit("要测量的通道号列表:")<< channelList ;
    //根据问题 改变绝缘电压后 第一次测量会有问题 提前进行一次测量抛掉数据
    {
        bool isC;float rv;
        quint8 vol_ = GlobalCommon::GetInstance().insulationVoltage();
        resTester->runTest(CMD_INS,&isC,&rv,vol_);
    }


    int currentChannel = 0;
    int totalChannel = channelList.size();
    for(int i=0; i<channelList.size(); i++)
    {
        //读入通道编号
        currentChannel = channelList.at(i)+1;
        //设置通道出于测量状态

        emit currentInsTest(currentChannel);
        qDebug()<< QString::fromLocal8Bit("开始绝缘测试，通道: ") << currentChannel <<  QString::fromLocal8Bit("剩余待检测通道数:") << totalChannel;
        //闭合继电器
        doController->selectChannel(currentChannel);
        //选择通道  400101 = 通道号
        resTester->setCurrentChannel(currentChannel);
        //绝缘命令  400102 = 0f
        bool isCredible = false;
        float resvalue = 0;

        quint8 vol_ = GlobalCommon::GetInstance().insulationVoltage();
        qint8 errorcode = resTester->runTest(CMD_INS,&isCredible,&resvalue,vol_);
        errorcode = resTester->runTest(CMD_INS,&isCredible,&resvalue,vol_);

        //除去绝缘自检阻值
        GlobalCommon::GetInstance().getSelfTestRValue();
        //resvalue -= GlobalCommon::GetInstance().insSelfTVal(currentChannel);

        if(isCredible !=true) //测试结果不可信
        {
            GlobalCommon::GetInstance().setChannelInsValReliable(currentChannel,false);
            emit channelInsTestRes(currentChannel,false,resvalue);

            if(GlobalCommon::GetInstance().getTelFunItem() == 4)
            {
                if(errorcode == -6) //线路干扰 结束检测
                {
                    qDebug() << QString::fromLocal8Bit("线路干扰,结束检测,请检查硬件!");
                    goto the_end;
                }else if(errorcode == -5)
                {
                    //n号桥路测试，通道  询问是否继续
                    qDebug() << QString::fromLocal8Bit("%1号通道测量失败,是否继续?").arg(currentChannel);
                }
            }
            else
            {
                if(errorcode == -6) //线路干扰 结束检测
                {
                    int q = ShowMessageBoxFromOtherThread::show("测量失败",QString("线路干扰,结束检测,请检查硬件!"),QMessageBox::Critical);
                    qDebug() << QString::fromLocal8Bit("线路干扰,结束检测,请检查硬件!");
                    goto the_end;
                }else if(errorcode == -5)
                {
                    //n号桥路测试，通道  询问是否继续
                    int q = ShowMessageBoxFromOtherThread::show("测量失败",QString("%1号通道测量失败,是否继续?").arg(currentChannel),QMessageBox::Question);
                    qDebug() << QString::fromLocal8Bit("%1号通道测量失败,是否继续?").arg(currentChannel);
                    if(q == QMessageBox::No)  //停止
                    {
                        qDebug() << "No";
                        goto the_end;
                    }else{
                        qDebug() << "Yes";
                    }
                }
            }
        }else
        {
            GlobalCommon::GetInstance().setChannelInsValReliable(currentChannel,true);
            GlobalCommon::GetInstance().setChannelInsVal(currentChannel,resvalue);
            emit channelInsTestRes(currentChannel,true,resvalue);
        }
        qDebug()<< QString::fromLocal8Bit("绝缘测试，通道: ") << currentChannel  << currentChannel << " value :" << resvalue;
        QAppHelper::delayms(DELAYTIME);//延时1秒
    }
    //emit insulationTestOver();

the_end:
    emit insulationTestOver();
    qDebug() << __FUNCTION__ << "绝缘测试 结束";
}

void BackWorkThread::reltest()
{
    doController->setWorkMode(DoController::Bridge);
    //todo 从GlobalCommon中获取要检测的通道数量 和编号
    //todo 加锁

    QVector<int> channelList = GlobalCommon::GetInstance().selectedChannelList();
    qDebug()<< QString::fromLocal8Bit("泄放部分开始") << channelList ;

    //泄放
    doController->setWorkMode(DoController::Bridge);//继电器设置为泄放模式
    //qint8 res = 0;//命令执行结果
    int currentChannel = 0;
    int totalChannel = channelList.size();
    for(int i=0; i<channelList.size(); i++)
    {
        //读入通道编号
        currentChannel = channelList.at(i)+1;
        //设置通道出于测量状态
        emit currentReleaseTest(currentChannel);
        qDebug()<< QString::fromLocal8Bit("开始泄放，通道: ") << currentChannel <<  QString::fromLocal8Bit("剩余待检测通道数:") << totalChannel;
        //闭合继电器
        doController->selectChannel(currentChannel);
        //选择通道  400101 = 通道号
        resTester->setCurrentChannel(currentChannel);
        //桥路命令  400102 = 7
        bool isCredible = false;
        float resvalue = 0;

        qint8 errorcode = resTester->runTest(CMD_RES,&isCredible,&resvalue);
        errorcode = resTester->runTest(CMD_RES,&isCredible,&resvalue);

        if(isCredible !=true) //测试结果不可信
        {
            GlobalCommon::GetInstance().setChannelReleaseValReliable(currentChannel,false);
            emit channelReleaseTestRes(currentChannel,false,resvalue);
            if(errorcode == -6) //线路干扰 结束检测
            {
                ShowMessageBoxFromOtherThread::show("测量失败",QString(tr("线路干扰,结束检测,请检查硬件!")),QMessageBox::Critical);
                qDebug() << QString::fromLocal8Bit("线路干扰,结束检测,请检查硬件!");
                // return;
                goto the_end;
            }else if(errorcode == -5)
            {
                //n号通道自检失败  询问是否继续
                int q = ShowMessageBoxFromOtherThread::show("测量失败",QString(tr("%1号通道测量失败,是否继续?").arg(currentChannel)),QMessageBox::Question);
                qDebug() << QString(tr("%1号通道测量失败,是否继续?").arg(currentChannel));
                if(q == QMessageBox::No)  //停止
                {
                    qDebug() << "No";
                    goto the_end;
                }else{
                    qDebug() << "Yes";
                }
            }
        }else
        {
            GlobalCommon::GetInstance().setChannelReleaseValReliable(currentChannel,true);
            GlobalCommon::GetInstance().setChannelReleaseVal(currentChannel,resvalue);
            emit channelReleaseTestRes(currentChannel,true,resvalue);
        }
        qDebug()<< QString::fromLocal8Bit("泄放，通道: " )<< currentChannel  << currentChannel << " value :" << resvalue;
        QAppHelper::delayms(DELAYTIME);//延时1秒
    }
the_end:
    emit releaseOver(); //泄放自检结束
    qDebug() << __FUNCTION__ << "泄放 结束";
}




void BackWorkThread::channeltest()
{
    emit setDoWorkMode(DoController::Bridge);
    for(int i=0; i < 30; i++)
    {
        doController->selectChannel(i);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QThread::sleep(1);
#else
        QElapsedTimer t;
        t.start();
        while(t.elapsed()<1000)
            QCoreApplication::processEvents();
#endif

        emit process(m_currentWork,i,double(0.1268*rand()));
        qDebug() << "processed()";
    }
}

void BackWorkThread::cutoff()
{
    doController->setWorkMode(DoController::Cutoff);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QThread::sleep(1);
#else
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<1000)
        QCoreApplication::processEvents();
#endif
    emit cutoffOver();
}

void BackWorkThread::functest()
{
    doController->setWorkMode(DoController::Bridge);
    qDebug() << __PRETTY_FUNCTION__ << "Bridge";

    for(int i=1;i<31; i++)
    {
        qDebug() << "ch:" << i;
        doController->selectChannel(i);
        QAppHelper::delayms(300);
    }

    qDebug() << __PRETTY_FUNCTION__ << "Bridge end"  ;

    doController->setWorkMode(DoController::Ins);
    qDebug() << __PRETTY_FUNCTION__ << "Ins";

    for(int i=1;i<31; i++)
    {
        qDebug() << "ch:" << i;
        doController->selectChannel(i);
        QAppHelper::delayms(300);
    }

    qDebug() << __PRETTY_FUNCTION__ << "Ins end"  ;
}


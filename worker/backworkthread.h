#ifndef SELFTESTTHREAD_H
#define SELFTESTTHREAD_H

#include "rthreadobj.h"
#include "docontroller.h"
#include "restester.h"

#include <QPointer>
#include <QSettings>
#include <QStringList>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QElapsedTimer>
#include <QCoreApplication>
#endif

#include "gpio.h"

class GlobalCommon;

class BackWorkThread : public  rThreadObj
{
    Q_OBJECT
public:
    enum eWorkType{
        PWONSELFTEST = 1,  //自检
        PRECHSELFTEST,     //按下通道自检时候发送的命令（未执行）
        CHSELFTEST,        //通道自检（执行）
        CHSELFTESTINS,        //绝缘自检
        CUTOFF,            //切断
        BRITEST,           //桥路测量
        INSTEST,           //绝缘测量
        RELTEST,           //泄放测量
        FUNCTEST, //用于功能测试
        UnKown = -1
    };
    Q_ENUMS(eWorkType)

    BackWorkThread();
    ~BackWorkThread();
    void run();

    void start(eWorkType a = UnKown);
signals:
    //用于跨线程访问
    void initconfig(int dev,QString qc);  // i==1 控制器  i==2 检测仪
    void selectChannel(int);
    void setDoWorkMode(DoController::eChannelType);

    void sendPowerOnSelfTestCMD();

    //通道自检 全部完成
    void selfTestOver(int type);
    //通道自检正在检测的通道
    void currentChannelSelfTest(int a);
    //通道自检检测结果汇报 type 0 桥路 1 绝缘
    void channelSeflTestRes(int type,int channel,bool isok,float dat);
    //通道检测的结果
    void process(BackWorkThread::eWorkType wk,int channel,double res);
    //开机检测的结果  i==1 控制器  i==2 检测仪
    void process(BackWorkThread::eWorkType wk,int i,bool res);
    //桥路检测正在检测的通道
    void currentBridgeTest(int a);
    //桥路检测的结果回报
    void channelBridgeTestRes(int channel,bool isok,float dat);
    //绝缘检测正在检测的通道
    void currentInsTest(int a);
    //绝缘检测的结果回报
    void channelInsTestRes(int channel,bool isok,float dat);
    //泄放检测正在检测的通道
    void currentReleaseTest(int a);
    //泄放检测的结果回报
    void channelReleaseTestRes(int channel,bool isok,float dat);

    void bridgeTestOver();
    void insulationTestOver();
    void releaseOver();

    void cutoffOver();

    void running();
    void runover();
private:
    void powerOnSelfTest();    
    void preChannelSelfTest();//按下通道自检即设置功能函数
    void channelSelfTest(int type);
    void bridgetest();
    void instest();
    void reltest();
    void channeltest(); //测试用 已废弃
    void cutoff();
    void functest();
private slots:

private:
    eWorkType m_currentWork;
    QMutex m_mtx;

    QPointer<DoController> doController;
    QPointer<ResTester> resTester;
    QStringList restesterconfig;
    QStringList docontrollerconfig;
};

#endif // SELFTESTTHREAD_H

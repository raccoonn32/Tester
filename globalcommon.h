#ifndef GLOBALCOMMON_H
#define GLOBALCOMMON_H

#include <QObject>
#include <QSettings>
#include "channelindicatorbutton.h"
#include <QtAlgorithms>
#include "docontroller.h"
#include "backworkthread.h"

#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QFileDialog>
#include <QDateTime>

#define SELFTEST_TIMEOUT (60)   //Sec
#define PORT    8888


//主页面中的字符串及 页面ID定义
#define STRNOTDECECTED   (tr("待检测")
#define STRDECECTING     (tr("检测中")
#define STRNORMAL        (tr("正常"))
#define STRFAIL          (tr("错误"))


#define SELFTEST_PAGE     0
#define CHTEST_PAGE       1
#define VOLSET_PAGE       2
#define CHSEL_PAGE        3
#define BRIDGE_PAGE       4
#define INSULATION_PAGE   5
//#define RELEASE_PAGE      6
#define CUTOFF_PAGE       6
#define CHTEST_PAGE2      7

#define TOTALCHANNEL      30

class GlobalCommon : public QObject
{
    Q_OBJECT

public:
    enum SelfTestResult {     //自检结果
        failur = 0,
        controllerOk,      //控制器正常
        detectorOk,        //检测仪正常
        controllerFailur,  //控制器正常
        detectorFailur,    //检测仪正常
        success,
        untest             //未检测
    };
    Q_ENUMS(SelfTestResult)

private:
    Q_DISABLE_COPY(GlobalCommon)
    explicit GlobalCommon(QObject *parent = NULL);

signals:
    void pwSelfTestResultChanged();
    void britestProgessChanged(int channel);
    void channelTestProgessChange(int channel);

    void running();
    void runover();

    void currentChannelSelfTest(int channel);
    void channelSeflTestRes(int type,int channel,bool isok,float dat);
    void selfTestOver(int type);
    void bridgeTestOver();
    void insulationTestOver();
    void releaseOver();
    void cutoffOver();
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

    void insulationVoltageChanged(quint8 v);
public:
    SelfTestResult controllerSelfTestResult();
    SelfTestResult detectorSelfTestResult();

    bool ChannelIsInsAvailable(int channel);
    void setChannelIsInsAvailable(int channel, bool b);
    bool ChannelIsBrgAvailable(int channel);
    void setChannelIsBrgAvailable(int channel, bool b);
    void setChannelSelected(int channel);
    void setChannelUnselected(int channel);
    QVector<int> selectedChannelList() const;
    void setChannelType(int chtype);

private:


public:
    static GlobalCommon& GetInstance();
    void startWork(BackWorkThread::eWorkType wk);
    void setInsulationVoltage(quint8 vol);
    quint8 insulationVoltage();

    void setControllerSelfTestResult(GlobalCommon::SelfTestResult sta);
    void setDetectorSelfTestResult(GlobalCommon::SelfTestResult sta);

    void setChannelBridgeVal(int channel , double d);
    void setChannelInsVal(int channel , double d);
    void setChannelBridgeSelfTVal(int channel , double d);
    void setChannelInsSelfTVal(int channel , double d);
    void setChannelReleaseVal(int channel , double d);

    float bridgeSelfTVal(int channel);
    float insSelfTVal(int channel);
    float bridgeVal(int channel);
    float insVal(int channel);
    float releaseVal(int channel);

    void setChannelBridgeValReliable(int channel , bool d);
    void setChannelInsValReliable(int channel , bool d);

    bool bridgeValReliable(int channel);
    bool insValReliable(int channel);

    void setChannelReleaseValReliable(int channel, bool d);
    bool releaseValReliable(int channel);

    void setRecordDir(QDir dir);
    QDir getRecordDir();
    void saveResultFile(BackWorkThread::eWorkType typede);

    void setTelFunItem(int funsel);
    int getTelFunItem();

    void settingSelfTest(int type); //保存自检线阻值
    void getSelfTestRValue();       //获取桥路和绝缘自检线组值

private:
    SelfTestResult m_controller_testResult; //自检控制器状态
    SelfTestResult m_detector_testResult;//自检检测仪状态

    QVector<bool> m_channelselftestBrgispass; //用于保存通道列表的桥路状态 是否通过自检
    QVector<bool> m_channelselftestInsispass; //用于保存通道列表的绝缘状态 是否通过自检
    QVector<int> m_brgChannelselected;       //用于保存通道列表的状态 被选中的通道号
    QVector<int> m_insChannelselected;  //用于保存绝缘通道选择的通道号**

    quint8 m_insulation_voltage;//绝缘电压
    QVector<double> m_bridgeSelftestVal;  //桥路自检测量值
    QVector<double> m_insulatSelftestVal; //绝缘自检测量值
    QVector<double> m_bridgeVal;  //桥路测量值
    QVector<double> m_insulatVal; //绝缘测量值
    QVector<double> m_releaseVal; //泄放测量值
    QVector<bool>   m_bridgeValReliable;
    QVector<bool>   m_insulatValReliable;
    QVector<bool>   m_releaseValReliable;

    //后台线程的
    BackWorkThread *backWorkObj;
    QThread *backThread;
    DoController *doController;
    QMutex m_GMutex;
    QDir m_record_dir;

    int m_channelType;//通道类型：绝缘or桥路
    int m_telFunItem;

public slots:
    void onThreadMessage(QString a);
    void onThreadProcess(BackWorkThread::eWorkType wk,int channel,double data);
    void onThreadProcess(BackWorkThread::eWorkType wk,int dev,bool sta);

    //测试用函数
    void test();
};

#endif // GLOBALCOMMON_H

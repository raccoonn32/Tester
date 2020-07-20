#ifndef DOCONTROLLER_H
#define DOCONTROLLER_H

#include <QObject>
#include <QVector>
#include <QPair>
#include <QMutex>
#include <QEventLoop>
#include <QThread>
#include <QMetaType>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QElapsedTimer>
#include <QCoreApplication>
#include <stdint.h>
#endif

#include "qmodbusmaster.h"
#include "qmodbusbits.h"


using namespace Modbus;

class DoController : public QObject
{
    Q_OBJECT
public:
    explicit DoController(const QString &port,int baud = 9600,char pairty = 'N',int databits = 8,int stopbits = 1 ,QObject *parent = NULL);
    ~DoController();
    //通道类型
    enum eChannelType{
        SelfTest = 1, //弃用模式
        ChannelTest,  //弃用
        Ins,
        Bridge,
        Release,
        Cutoff,
    };
    Q_ENUMS(eChannelType)

    enum eTaskType{
        eSetWorkMode = 1,
        eSetChannel
    };
    Q_ENUMS(eTaskType)

    struct ChannelConfig{
        int slaveid;
        int dochannel;
    };

    eChannelType workMode() const;
signals:

public:
    bool hasError();

public slots:
    void selectChannel(int channel);
    void setWorkMode(DoController::eChannelType type);

    void functest();
private slots:


private:
    ChannelConfig InsMode;    //配置绝缘总开关所属通道 设备
    ChannelConfig BridgeMode; //配置桥路总开关所属通道 设备
    eChannelType m_currentMode;//当前工作模式
    QVector<ChannelConfig> *SelfTestChList;
    QVector<ChannelConfig> *InsChList;
    QVector<ChannelConfig> *BridgeChList;
    QVector<ChannelConfig> *ReleaseChList;
    QModbusMaster *modbusMaster;
    void initConfig();

    bool m_hasError;
    //保存从机1 从机2 设备线圈状态
    QModbusBits m_bitsbuf_slave1;
    QModbusBits m_bitsbuf_slave2;
};

#endif // DOCONTROLLER_H

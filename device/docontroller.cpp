#include "docontroller.h"
#include "qapphelper.h"

#include "globalcommon.h"

#include <QThread>

DoController::DoController(const QString &port, int baud, char pairty, int databits, int stopbits, QObject *parent)
    :m_currentMode(SelfTest)
{
    initConfig();
    modbusMaster = new QModbusMaster(port.toStdString().c_str(),baud,pairty,databits,stopbits);
    modbusMaster->connect();

    m_hasError = modbusMaster->checkConnection();
    m_bitsbuf_slave1.resize(32);
    m_bitsbuf_slave2.resize(32);
    m_bitsbuf_slave1.setAddress(0);
    m_bitsbuf_slave2.setAddress(0);

//    qDebug() <<__FUNCTION__ << QThread::currentThreadId() << QThread::currentThread();
}

DoController::~DoController()
{
    qDebug() << __FUNCTION__;
}

DoController::eChannelType DoController::workMode() const
{
    return m_currentMode;
}

bool DoController::hasError(){
    return m_hasError;
}

//参数中的通道为用户层的通道
//该函数将找到对应通道控制模块的IO输出通道并完成设置 将写入所有的通道（包括 总开关）
//channel 1~30;
void DoController::selectChannel(int channel)
{
    if (m_currentMode == Cutoff)
        return;    
    int tc = channel;
    if(tc > TOTALCHANNEL || tc < 0)
    {
        return;
    }
    QModbusBits dev[2]; //临时缓冲 将写入到硬件
    {
        dev[0].resize(32);
        dev[1].resize(32);
        dev[0].setAddress(0);
        dev[1].setAddress(0);
        dev[0].fill(false);
        dev[1].fill(false); //所有的开关预设值为关闭
    }

    tc-=1;// 用数组下表作为通道编号

    if(m_currentMode == Ins) //绝缘模式
    {
        dev[InsMode.slaveid-1].setValue(InsMode.dochannel-1,true);
        dev[BridgeMode.slaveid-1].setValue(BridgeMode.dochannel-1,false);

        dev[InsChList->at(tc).slaveid-1].setValue(InsChList->at(tc).dochannel-1,true);
        qDebug() << "INS" << tc+1 << InsChList->at(tc).slaveid-1 << InsChList->at(tc).dochannel;
    }else if(m_currentMode == Bridge) //桥路模式
    {
        dev[InsMode.slaveid-1].setValue(InsMode.dochannel-1,false);
        dev[BridgeMode.slaveid-1].setValue(BridgeMode.dochannel-1,true);

        dev[BridgeChList->at(tc).slaveid-1].setValue(BridgeChList->at(tc).dochannel-1,true);

        qDebug() << "BRG" << tc+1 << BridgeChList->at(tc).slaveid-1 << BridgeChList->at(tc).dochannel;
    }else if(m_currentMode == Release) //泄放模式
    {
        dev[InsMode.slaveid-1].setValue(InsMode.dochannel-1,false);
        dev[BridgeMode.slaveid-1].setValue(BridgeMode.dochannel-1,false);

        dev[BridgeChList->at(tc).slaveid-1].setValue(BridgeChList->at(tc).dochannel-1,true);
    }

    modbusMaster->setSlave(1);      //地址
    modbusMaster->writeBits(dev[0]);
    QAppHelper::delayms(500);
    modbusMaster->setSlave(2);
    modbusMaster->writeBits(dev[1]);
}


//设置工作模式
void DoController::setWorkMode(DoController::eChannelType type)
{
    qDebug() << __FUNCTION__ << type << QThread::currentThread();
    QModbusBits bit;
    if(type == Ins) //绝缘模式
    {
        modbusMaster->setSlave(BridgeMode.slaveid);
        bit.setAddress(BridgeMode.dochannel-1);
        bit.setValue(0,false);
        modbusMaster->writeBits(bit);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QThread::msleep(500);
#else
        QElapsedTimer t;
        t.start();
        while(t.elapsed()<500)
            QCoreApplication::processEvents();
#endif

        modbusMaster->setSlave(InsMode.slaveid);
        bit.setAddress(InsMode.dochannel-1);
        bit.setValue(0,true);
        modbusMaster->writeBits(bit);

    }else if(type == Bridge) //桥路模式
    {
        modbusMaster->setSlave(InsMode.slaveid);
        bit.setAddress(InsMode.dochannel-1);
        bit.setValue(0,false);
        modbusMaster->writeBits(bit);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QThread::msleep(500);
#else
        QElapsedTimer t;
        t.start();
        while(t.elapsed()<500)
            QCoreApplication::processEvents();
#endif

        modbusMaster->setSlave(BridgeMode.slaveid);
        bit.setAddress(BridgeMode.dochannel-1);
        bit.setValue(0,true);
        modbusMaster->writeBits(bit);
    }else if(type == Release) //泄放模式
    {
        modbusMaster->setSlave(InsMode.slaveid);
        bit.setAddress(InsMode.dochannel-1);
        bit.setValue(0,false);
        modbusMaster->writeBits(bit);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QThread::msleep(500);
#else
        QElapsedTimer t;
        t.start();
        while(t.elapsed()<500)
            QCoreApplication::processEvents();
#endif

        modbusMaster->setSlave(BridgeMode.slaveid);
        bit.setAddress(BridgeMode.dochannel-1);
        bit.setValue(0,false);
        modbusMaster->writeBits(bit);
    }else if(type == Cutoff) //切断所有通道
    {
        bit.fill(false,32);
        modbusMaster->setSlave(1);
        modbusMaster->writeBits(bit);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QThread::msleep(500);
#else
        QElapsedTimer t;
        t.start();
        while(t.elapsed()<500)
            QCoreApplication::processEvents();
#endif
        modbusMaster->setSlave(2);
        modbusMaster->writeBits(bit);
    }

    if(modbusMaster->lastError().isValid())
    {
        qDebug() << __FUNCTION__ << "fault";
    }else
        m_currentMode = type;
}

void DoController::functest()
{
    QByteArray ba;
#if !defined(Q_OS_WIN)
    u_int8_t raw_req[] = { 0x01, 0x03, 0x02, 0x10, 0x00, 0x06, 0xC5, 0xB5 };
#else
    uint8_t raw_req[] = { 0x01, 0x03, 0x02, 0x10, 0x00, 0x06, 0xC5, 0xB5 };
#endif

    ba.append((const char*)raw_req,8);
    QByteArray rsp;
    modbusMaster->writeAndReadRawData(ba,rsp);

    qDebug() <<__FUNCTION__ << rsp.toHex();
}

void DoController::initConfig()
{
    InsMode.slaveid = 1;//对应控制模块1
    InsMode.dochannel = 9;//对应控制模块DO1

    BridgeMode.slaveid = 1;
    BridgeMode.dochannel = 1;

    BridgeChList = new QVector<ChannelConfig>();
    InsChList = new QVector<ChannelConfig>();
    //SelfTestChList = new QVector<ChannelConfig>();
    //ReleaseChList = new QVector<ChannelConfig>();

    ChannelConfig tp;
    tp.slaveid = 1;
    for(int i = 0; i< 7;i++)
    {
        tp.dochannel = 2+i;
        BridgeChList->append(tp);
    }
    for(int i = 0; i< 7;i++)
    {
        tp.dochannel = 10+i;
        InsChList->append(tp);
    }
    for(int i = 0; i< 8;i++)
    {
        tp.dochannel = 17+i;
        BridgeChList->append(tp);
    }
    for(int i = 0; i< 8;i++)
    {
        tp.dochannel = 25+i;
        InsChList->append(tp);
    }

    tp.slaveid = 2;
    for(int i = 0; i< 8;i++)
    {
        tp.dochannel = 1+i;
        BridgeChList->append(tp);
    }
    for(int i = 0; i< 8;i++)
    {
        tp.dochannel = 9+i;
        InsChList->append(tp);
    }
    for(int i = 0; i< 7;i++)
    {
        tp.dochannel = 17+i;
        BridgeChList->append(tp);
    }
    for(int i = 0; i< 7;i++)
    {
        tp.dochannel = 25+i;
        InsChList->append(tp);
    }


    qDebug() << "BRG -> DOCHLIST" << BridgeChList->size();
    for(int i=0;i<InsChList->size();i++)
    {
        qDebug() << "BRG"  << i+1 << BridgeChList->at(i).slaveid << BridgeChList->at(i).dochannel ;
    }

    qDebug() << "INS -> DOCHLIST" << InsChList->size();
    for(int i=0;i<InsChList->size();i++)
    {
        qDebug() << "INS"  << i+1 << InsChList->at(i).slaveid << InsChList->at(i).dochannel ;
    }

    //注意泄放测量 通道按照桥路的方式 但 桥路总开关不开
    if(0){
        qDebug() << "通道配置表  绝缘电阻测试" << InsChList->size();
        qDebug() << "通道 DO  SLAVE";
        for(int i = 0;i< InsChList->size();i++)
        {
            qDebug() << i+1 << InsChList->at(i).dochannel << InsChList->at(i).slaveid;
        }
        qDebug() << "通道配置表  桥路电阻测试" << BridgeChList->size();
        for(int i = 0;i< BridgeChList->size();i++)
        {
            qDebug()<< i+1 << BridgeChList->at(i).dochannel << BridgeChList->at(i).slaveid;
        }
    }
}

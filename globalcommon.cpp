#include "globalcommon.h"
#include "qapphelper.h"

GlobalCommon::GlobalCommon(QObject *parent) : QObject(parent)
{
    {
        m_controller_testResult = untest;
        m_detector_testResult = untest;
        m_channelselftestInsispass.fill(false,TOTALCHANNEL);   //默认全部未通过自检
        m_channelselftestBrgispass.fill(false,TOTALCHANNEL);   //默认全部未通过自检
        m_brgChannelselected.resize(0);
        m_insChannelselected.resize(0);
        m_bridgeSelftestVal.fill(0,TOTALCHANNEL);
        m_insulatSelftestVal.fill(0,TOTALCHANNEL);
        m_releaseVal.fill(0,TOTALCHANNEL);
        m_bridgeVal.fill(0,TOTALCHANNEL);
        m_insulatVal.fill(0,TOTALCHANNEL);
        m_bridgeValReliable.fill(false,TOTALCHANNEL);
        m_insulatValReliable.fill(false,TOTALCHANNEL);
        m_releaseValReliable.fill(false,TOTALCHANNEL);
        m_insulation_voltage = 500;  //默认500V绝缘电压

        m_channelType = 0;//默认显示桥路通道选择
        m_telFunItem = 0;
    }

    backThread = new QThread();

    backWorkObj = new BackWorkThread();
    backWorkObj->moveToThread(backThread);

    //用于跨线程访问
    //connect(backWorkObj,&BackWorkThread::message,this,&GlobalCommon::onThreadMessage);
    connect(backWorkObj,SIGNAL(message(QString)),this,SLOT(onThreadMessage(QString)));
    connect(backWorkObj,SIGNAL(process(BackWorkThread::eWorkType,int,double))
            ,this,SLOT(onThreadProcess(BackWorkThread::eWorkType,int,double))
            ,Qt::QueuedConnection);

    connect(backWorkObj,SIGNAL(process(BackWorkThread::eWorkType,int,bool))
            ,this,SLOT(onThreadProcess(BackWorkThread::eWorkType,int,bool))
            ,Qt::QueuedConnection);

    connect(backWorkObj,SIGNAL(currentChannelSelfTest(int)),this,SIGNAL(currentChannelSelfTest(int)));
    connect(backWorkObj,SIGNAL(channelSeflTestRes(int,int,bool,float)),
            this,SIGNAL(channelSeflTestRes(int,int,bool,float)));
    connect(backWorkObj,SIGNAL(selfTestOver(int)),this,SIGNAL(selfTestOver(int)));

    connect(backWorkObj,SIGNAL(currentBridgeTest(int)),this,SIGNAL(currentBridgeTest(int)));
    connect(backWorkObj,SIGNAL(currentInsTest(int)),this,SIGNAL(currentInsTest(int)));
    connect(backWorkObj,SIGNAL(currentReleaseTest(int)),this,SIGNAL(currentReleaseTest(int)));

    connect(backWorkObj,SIGNAL(channelBridgeTestRes(int,bool,float)),this,SIGNAL(channelBridgeTestRes(int,bool,float)));
    connect(backWorkObj,SIGNAL(channelInsTestRes(int,bool,float)),this,SIGNAL(channelInsTestRes(int,bool,float)));
    connect(backWorkObj,SIGNAL(channelReleaseTestRes(int,bool,float)),this,SIGNAL(channelReleaseTestRes(int,bool,float)));

    connect(backWorkObj,SIGNAL(insulationTestOver()),this,SIGNAL(insulationTestOver()));
    connect(backWorkObj,SIGNAL(bridgeTestOver()),this,SIGNAL(bridgeTestOver()));
    connect(backWorkObj,SIGNAL(releaseOver()),this,SIGNAL(releaseOver()));

    connect(backWorkObj,SIGNAL(cutoffOver()),this,SIGNAL(cutoffOver()));

    connect(backWorkObj,SIGNAL(running()),this,SIGNAL(running()));
    connect(backWorkObj,SIGNAL(runover()),this,SIGNAL(runover()));
    backThread->start();
}

GlobalCommon &GlobalCommon::GetInstance()
{
    static GlobalCommon s_gCommon;   //局部静态变量
    return s_gCommon;
}

void GlobalCommon::startWork(BackWorkThread::eWorkType wk)
{
    backWorkObj->start(wk);
}

void GlobalCommon::setInsulationVoltage(quint8 vol){
    //设置绝缘电压
    qDebug() << /*QString::fromLocal8Bit*/tr("设置绝缘电压") << QString::number(vol,16) << "@BCDCODE" << vol;
    m_insulation_voltage = vol;
    emit insulationVoltageChanged(m_insulation_voltage);
}


quint8 GlobalCommon::insulationVoltage()
{
    return m_insulation_voltage;
}

void GlobalCommon::onThreadMessage(QString a)
{
    qDebug() << a;
}

void GlobalCommon::onThreadProcess(BackWorkThread::eWorkType wk, int dev, bool sta)
{
    if(wk == BackWorkThread::PWONSELFTEST)//开机自检
    {
        if(dev == 1)//控制器
        {
            m_GMutex.lock();
            m_controller_testResult = sta? controllerOk: controllerFailur;
            m_GMutex.unlock();

        }else if(dev == 2)//检测仪
        {
            m_GMutex.lock();
            m_detector_testResult = sta? detectorOk: detectorFailur;
            m_GMutex.unlock();
        }
        emit pwSelfTestResultChanged();
    }
}

/**
 * @brief GlobalCommon::saveResultFile
 * @param typede
 * 保存每次的记录文件
 */
void GlobalCommon::saveResultFile(BackWorkThread::eWorkType typede)
{
    qDebug() << __FUNCTION__ << "start workmode: " << typede;
    QString typestr = "";

    switch(typede){
    case BackWorkThread::CHSELFTESTINS:
        typestr = tr("jueyuantest");
        break;
    case BackWorkThread::CHSELFTEST:
        typestr = tr("qiaolutest");
        break;
    case BackWorkThread::BRITEST:
        typestr = tr("qiaolu");
        break;
    case BackWorkThread::INSTEST:
        typestr = tr("jueyuan");
        break;
    case BackWorkThread::RELTEST:
        typestr = tr("xiefang");
        break;

    default:
        return;
    }

    QDateTime time = QDateTime::currentDateTime();
    QString dirname = time.toString("yyyy-MM-dd_hh-mm-ss");
    qDebug() << "time dirname:" << dirname;
    QDir dir = QDir::currentPath();
    if(QDir(dir.absolutePath() + "/record").exists() != true)
        dir.mkdir("record");
    dir.cd("record");

    QString filepath = dir.absolutePath() + "/" + dirname + "_" + typestr;


//    QString path = m_record_dir.absolutePath() + "/";
//    QString filepath = path  + typestr;
    int i=0;
    do{
        filepath = (i == 0)? filepath : filepath+ "_" +QString::number(i);
        if( QFile::exists(filepath) != true )
        {
            break;
        }
    }while(++i);
    filepath += ".csv";
    qDebug() << "save record file name : " << filepath;

    QFile file(filepath);
    QTextStream textStream;
    textStream.setDevice(&file);
    textStream.setCodec("UTF-8");

    if(!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)){
        //        QMessageBox::information(this,"创建文件错误",QString("创建文件错误，请检查文件系统 \n 预创建文件路径为 %1").arg(filepath),NULL);
        return;
    }

    if(typede == BackWorkThread::CHSELFTEST)
    {
        textStream <<tr("channelNum") << "," << tr("isCredible") << "," << tr("RValue") << "\n";
        for(int x=0; x < TOTALCHANNEL; x++)
        {
            int channelnumber = x+1;
            QString dataStr = QString::number(channelnumber) + ",";
            QString reliablex = GlobalCommon::GetInstance().ChannelIsBrgAvailable(channelnumber) ? "true" : "false";
            dataStr += reliablex + ",";
            dataStr += QString::number(GlobalCommon::GetInstance().bridgeSelfTVal(channelnumber)) + "\n";
            textStream << dataStr;
        }
    }
    if(typede == BackWorkThread::CHSELFTESTINS)
    {
        textStream <<tr("channelNum") << "," << tr("isCredible") << "," << tr("RValue") << "\n";
        for(int x=0; x < TOTALCHANNEL; x++)
        {
            int channelnumber = x+1;
            QString dataStr = QString::number(channelnumber) + ",";
            QString reliable = GlobalCommon::GetInstance().ChannelIsInsAvailable(channelnumber) ? "true" : "false";
            dataStr += reliable + ",";
            dataStr += QString::number(GlobalCommon::GetInstance().insSelfTVal(channelnumber)/1000)+ "\n";
            textStream << dataStr;
        }
    }

    //输出表头
    if(typede != BackWorkThread::CHSELFTEST && typede != BackWorkThread::CHSELFTESTINS)
    {
//        if(typede == BackWorkThread::INSTEST)
//            textStream << "绝缘测量" << "," << "绝缘电压" << "," << QString::number(GlobalCommon::GetInstance().insulationVoltage(),16) << "\n";
//        else
//            textStream << "typestr" << "," << "NA" << "," << "NA" << "\n";
        textStream << tr("channelNum") << "," << tr("isCredible") << "," << tr("RValue") << "\n";

        if(typede == BackWorkThread::BRITEST)
        {
            for(int x=0; x < TOTALCHANNEL; x++)
            {
                int channelnumber = x+1;
                QString dataStr = QString::number(channelnumber) + ",";
                QString reliable = GlobalCommon::GetInstance().bridgeValReliable(channelnumber) ? "true" : "false";
                dataStr += reliable + ",";
                dataStr += QString::number(GlobalCommon::GetInstance().bridgeVal(channelnumber)) + "\n";
                textStream << dataStr;
            }
        }else if(typede == BackWorkThread::INSTEST)
        {
            for(int x=0; x < TOTALCHANNEL; x++)
            {
                int channelnumber = x+1;
                QString dataStr = QString::number(channelnumber) + ",";
                QString reliable = GlobalCommon::GetInstance().insValReliable(channelnumber) ? "true" : "false";
                dataStr += reliable + ",";
                dataStr += QString::number(GlobalCommon::GetInstance().insVal(channelnumber)) + "\n";
                textStream << dataStr;
            }
        }else if(typede == BackWorkThread::RELTEST)
        {
            for(int x=0; x < TOTALCHANNEL; x++)
            {
                int channelnumber = x+1;
                QString dataStr = QString::number(channelnumber) + ",";
                QString reliable = GlobalCommon::GetInstance().releaseValReliable(channelnumber) ? "true" : "false";
                dataStr += reliable + ",";
                dataStr += QString::number(GlobalCommon::GetInstance().releaseVal(channelnumber)) + "\n";
                textStream << dataStr;
            }
        }
    }


    file.close();
    qDebug() << __FUNCTION__ << "end";
}

void GlobalCommon::test(){
    backWorkObj->start(BackWorkThread::PWONSELFTEST);
}

void GlobalCommon::onThreadProcess(BackWorkThread::eWorkType wk, int channel, double data)
{
    qDebug() << __FILE__ << __FUNCTION__ << wk << "channel " << channel << data;
    if(wk == BackWorkThread::BRITEST)//桥路测量
    {
        emit britestProgessChanged(channel);
    }
    if(wk == BackWorkThread::CHSELFTEST)
    {
        emit channelTestProgessChange(channel);
    }
}


//获取通道检测结果
bool GlobalCommon::ChannelIsBrgAvailable(int channel)
{
    QMutexLocker locker(&m_GMutex);
    if(channel > 0 && channel <= TOTALCHANNEL)
    {
        return m_channelselftestBrgispass.at(channel-1);
    }return false;
}

void GlobalCommon::setChannelIsBrgAvailable(int channel,bool b)
{
    QMutexLocker locker(&m_GMutex);
    if(channel > 0 && channel <= TOTALCHANNEL)
    {
        m_channelselftestBrgispass[channel-1] = b;
    }
}

bool GlobalCommon::ChannelIsInsAvailable(int channel)
{
    QMutexLocker locker(&m_GMutex);
    if(channel > 0 && channel <= TOTALCHANNEL)
    {
        return m_channelselftestInsispass.at(channel-1);
    }return false;
}

void GlobalCommon::setChannelIsInsAvailable(int channel,bool b)
{
    QMutexLocker locker(&m_GMutex);
    if(channel > 0 && channel <= TOTALCHANNEL)
    {
        m_channelselftestInsispass[channel-1] = b;
    }
}

void GlobalCommon::setChannelSelected(int channel)
{
    QMutexLocker locker(&m_GMutex);
    if(m_channelType == 0){
        if (!m_brgChannelselected.contains(channel))
            m_brgChannelselected.append(channel);
        qSort(m_brgChannelselected);   //sort
    }
    else if(m_channelType == 1){
        if(!m_insChannelselected.contains(channel))
            m_insChannelselected.append(channel);
        qSort(m_insChannelselected);
    }
    qDebug() << "brg:" << m_brgChannelselected << "ins:" << m_insChannelselected;
}

void GlobalCommon::setChannelUnselected(int channel)
{
    QMutexLocker locker(&m_GMutex);
    if(m_channelType == 0){
        if (m_brgChannelselected.contains(channel))
        {
#if QT_VERSION > 0x050300
            m_brgChannelselected.removeOne(channel);
#else
            int i = m_brgChannelselected.indexOf(channel);
            m_brgChannelselected.remove(i);
#endif
        }
        qSort(m_brgChannelselected);
    }
    else if(m_channelType == 1){
        if(m_insChannelselected.contains(channel)){
#if QT_VERSION > 0x050300
            m_insChannelselected.removeOne(channel);
#else
            int i = m_insChannelselected.indexOf(channel);
            m_insChannelselected.remove(i);
#endif
        }
        qSort(m_insChannelselected);
    }

}

QVector<int> GlobalCommon::selectedChannelList() const
{
    if(m_channelType == 0)
        return m_brgChannelselected;
    else
        return m_insChannelselected;
}

//自检结果
GlobalCommon::SelfTestResult GlobalCommon::controllerSelfTestResult()
{
    QMutexLocker locker(&m_GMutex);
    return m_controller_testResult;
}

GlobalCommon::SelfTestResult GlobalCommon::detectorSelfTestResult()
{
    QMutexLocker locker(&m_GMutex);
    return m_detector_testResult;
}

void GlobalCommon::setControllerSelfTestResult(GlobalCommon::SelfTestResult sta)
{
    QMutexLocker locker(&m_GMutex);
    m_controller_testResult = sta;
}

void GlobalCommon::setDetectorSelfTestResult(GlobalCommon::SelfTestResult sta)
{
    QMutexLocker locker(&m_GMutex);
    m_detector_testResult = sta;
}

void  GlobalCommon::setChannelBridgeVal(int channel , double d)
{
    //桥路
    if(channel > 0 && channel < 31)
    {
        m_bridgeVal.replace(channel-1,d);
    }
}

void  GlobalCommon::setChannelInsVal(int channel , double d)
{
    //绝缘的默认单位是兆欧   绝缘模式/1000得到兆欧
    d /= 1000;
    if(channel > 0 && channel < 31)
    {
        m_insulatVal.replace(channel-1,d);
    }
}

void  GlobalCommon::setChannelBridgeSelfTVal(int channel , double d)
{
    //桥路  欧姆
    //d *= 1000;
    if(channel > 0 && channel < 31)
    {
        m_bridgeSelftestVal.replace(channel-1,d);
    }
}

void  GlobalCommon::setChannelInsSelfTVal(int channel , double d)
{
    //绝缘 自检测得是回路电阻 不是真的绝缘测量
    if(channel > 0 && channel < 31)
    {
        m_insulatSelftestVal.replace(channel-1,d);
    }
}

void GlobalCommon::setChannelReleaseVal(int channel, double d)
{
    //泄放的默认单位是兆欧   泄放模式/1000得到兆欧
    d /= 1000; d /= 1000;
    if(channel > 0 && channel < 31)
    {
        m_releaseVal.replace(channel-1,d);
    }
}

float  GlobalCommon::bridgeSelfTVal(int channel)
{
    if(channel > 0 && channel < 31)
    {
        return m_bridgeSelftestVal.at(channel-1);
    }
    else
        return 0;
}

float  GlobalCommon::insSelfTVal(int channel)
{
    if(channel > 0 && channel < 31)
    {
        return m_insulatSelftestVal.at(channel-1);
    }
}
float  GlobalCommon::bridgeVal(int channel)
{
    if(channel > 0 && channel < 31)
    {
        return m_bridgeVal.at(channel-1);
    }
}

float  GlobalCommon::insVal(int channel)
{
    if(channel > 0 && channel < 31)
    {
        return m_insulatVal.at(channel-1);
    }
}

float GlobalCommon::releaseVal(int channel)
{
    if(channel > 0 && channel < 31)
    {
        return m_releaseVal.at(channel-1);
    }
}

//该通道的数据是否可信
void GlobalCommon::setChannelBridgeValReliable(int channel , bool d)
{
    if(channel > 0 && channel < 31)
    {
        m_bridgeValReliable.replace(channel-1,d);
    }
}

void GlobalCommon::setChannelReleaseValReliable(int channel , bool d)
{
    if(channel > 0 && channel < 31)
    {
        m_releaseValReliable.replace(channel-1,d);
    }
}

void GlobalCommon::setChannelInsValReliable(int channel , bool d)
{
    if(channel > 0 && channel < 31)
    {
        m_insulatValReliable.replace(channel-1,d);
    }
}

bool GlobalCommon::bridgeValReliable(int channel)
{
    if(channel > 0 && channel < 31)
    {
        return m_bridgeValReliable.at(channel-1);
    }
}

bool GlobalCommon::insValReliable(int channel)
{
    if(channel > 0 && channel < 31)
    {
        return m_insulatValReliable.at(channel-1);
    }
}

bool GlobalCommon::releaseValReliable(int channel)
{
    if(channel > 0 && channel < 31)
    {
        return m_releaseValReliable.at(channel-1);
    }
}

void GlobalCommon::setRecordDir(QDir dir)
{
    m_record_dir = dir;
}

QDir GlobalCommon::getRecordDir()
{
    return m_record_dir;
}

void GlobalCommon::setTelFunItem(int funsel)
{
    m_telFunItem  = funsel;
}

int GlobalCommon::getTelFunItem()
{
    return m_telFunItem;
}

void GlobalCommon::settingSelfTest(int type)
{
    QDir dir = QDir::currentPath();
    if(QDir(dir.absolutePath() + "/RValue").exists() != true)
        dir.mkdir("RValue");
    dir.cd("RValue");

    QString filepath = "";
    if(type == 0)
    {
        filepath = dir.absolutePath() + "/bridge.csv";
    }
    else if(type == 1)
    {
        filepath = dir.absolutePath() + "/ins.csv";
    }

    QFile file(filepath);
    QTextStream textStream;
    textStream.setDevice(&file);
    file.remove();
    if(!file.open(QFile::ReadWrite)) return;

    if(type == 0)
    {
        for(int i = 0; i < TOTALCHANNEL; i++)
        {
            int channelnum = i+1;
            QString dataStr;
            dataStr = QString::number(GlobalCommon::GetInstance().bridgeSelfTVal(channelnum)) + "\n";
            textStream << dataStr;
        }
    }
    else
    {
        for(int i = 0; i < TOTALCHANNEL; i++)
        {
            int channelnum = i+1;
            QString dataStr;
            dataStr = QString::number(GlobalCommon::GetInstance().insSelfTVal(channelnum)/1000) + "\n";
            textStream << dataStr;

        }
    }

    file.close();
}

void GlobalCommon::getSelfTestRValue()
{
    QDir dir = QDir::currentPath();
    if(QDir(dir.absolutePath() + "/RValue").exists() == true)
    {
        dir.cd("RValue");
        QString filepath = dir.absolutePath() + "/bridge.csv";

        QFile file(filepath);
        if(file.open(QFile::ReadOnly))
        {
            int i = 0;
            while(!file.atEnd())
            {
                QByteArray bytearray = file.readLine();
                QList<QByteArray> list = bytearray.split('\n');
                m_bridgeSelftestVal[i++] =  list.at(0).toDouble();

                //qDebug() << "m_bridgeSelfTest" << i;
            }

        }
        file.close();

        filepath = dir.absolutePath() + "/ins.csv";
        QFile file2(filepath);
        if(file2.open(QFile::ReadOnly))
        {
            int i = 0;
            while(!file2.atEnd())
            {
                QByteArray bytearray = file2.readLine();
                QList<QByteArray> list = bytearray.split('\n');                
                m_insulatSelftestVal[i++] =  list.at(0).toDouble();
            }
        }
        file2.close();

    }

    for(int i = 0; i < TOTALCHANNEL; i++)
    {
        qDebug() <<"bridgeSelfTest:" << m_bridgeSelftestVal[i] << "insulationSelfTest:" << m_insulatSelftestVal[i];
    }

}

void GlobalCommon::setChannelType(int chtype)
{
    m_channelType = chtype;
}

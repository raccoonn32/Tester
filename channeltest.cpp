#include "channeltest.h"
#include "ui_channeltest.h"

#include "qapphelper.h"
#include "common/confirmw.h"

ChannelTest::ChannelTest(const QString title, int id, QWidget *parent) :
    cChildQWidget(title,id,parent),
    ui(new Ui::ChannelTest)
{
    ui->setupUi(this);
    QFont font = ui->lb_value1->font();
    font.setPixelSize(16);
    ui->lb_value1->setFont(font);
    ui->lb_value2->setFont(font);
    pLayout = new QGridLayout(ui->ChannelMap);
    for(int i=0;i<TOTALCHANNEL;i++)
    {
        ChannelIndicatorButton *tindicator = new ChannelIndicatorButton
                (ChannelIndicatorButton::Indicator,i+1,ChannelIndicatorButton::Default,ui->ChannelMap);
        tindicator->setFixedSize(50,50);
        m_channelIndicatorlist.append(tindicator);
        pLayout->addWidget(tindicator,i/10,i%10);

        //        tindicator->setState(ChannelIndicatorButton::estate(i%4));
    }

    ui->ChannelMap->setLayout(pLayout);
}


ChannelTest::~ChannelTest()
{
    delete pLayout;
    delete ui;
}

void ChannelTest::preExecute()
{
    GlobalCommon::GetInstance().startWork(BackWorkThread::PRECHSELFTEST);
}

void ChannelTest::currentDisplay(int id, QString vl1, QString vl2)
{
    setLabels(id,vl1,vl2);
}

void ChannelTest::setLabels(int id, QString vl1,QString vl2)
{
    ui->lb_No->setText(QString(tr("第%1通道自检电阻:  \n(欧姆)")).arg(id));
    ui->lb_value1->setText(vl1);
    ui->lb_value2->setText(vl2);
}

void ChannelTest::executeFunc(int type)
{
    ConfirmW cfw;

    if(0 == type){
        cfw.setText("系统将执行\"桥路通道自检\",确认吗?");
        int res = cfw.exec();
        if(res == QDialog::Accepted)
            GlobalCommon::GetInstance().startWork(BackWorkThread::CHSELFTEST);
    }
    else if(1 == type){
        cfw.setText("系统将执行\"绝缘通道自检\",确认吗?");
        int res = cfw.exec();
        if(res == QDialog::Accepted)
            GlobalCommon::GetInstance().startWork(BackWorkThread::CHSELFTESTINS);
    }
}

void ChannelTest::currentTest(int current)
{
    m_channelIndicatorlist.at(current-1)->setState(ChannelIndicatorButton::Testing);

}

void ChannelTest::onchannelSeflTestRes(int type, int channel, bool b, float dat)
{
    qDebug() << "检测完成" << type <<channel << b << dat;
    if(type == 0) //桥路测量
    {
        if(b == false){
            m_channelIndicatorlist.at(channel-1)->setState(ChannelIndicatorButton::Unavailable);
        }else{
            m_channelIndicatorlist.at(channel-1)->setState(ChannelIndicatorButton::Available);
        }
        GlobalCommon::GetInstance().setChannelIsBrgAvailable(channel,b);// 设置该通道桥路是否可用
    }else{//绝缘自检
        if(b == false){
            m_channelIndicatorlist.at(channel-1)->setState(ChannelIndicatorButton::Unavailable);
        }else{
            m_channelIndicatorlist.at(channel-1)->setState(ChannelIndicatorButton::Available);
        }
        //桥路和绝缘均通过才可用
        GlobalCommon::GetInstance().setChannelIsInsAvailable(channel,b);// 设置该通道绝缘是否可用
    }


    if(type == 0) //桥路
    {
        //桥路的默认单位是毫欧  d的单位是欧姆
        QString l1 = (QString::number(dat,'f',2) +"Ω");
        setLabels(channel,l1,"---");
    }else{
        float db = GlobalCommon::GetInstance().bridgeSelfTVal(channel);
        QString l1 = (QString::number(db,'f',2)+"Ω");
        //绝缘回路 的电阻 Ω
        dat /= 1000;
        QString l2 = (QString::number(dat,'f',2)+"MΩ");

        setLabels(channel,l1,l2);
    }
}

void ChannelTest::onSelfTestOver(int type)
{
    for(int i=0;i<TOTALCHANNEL;i++)
    {
        m_channelIndicatorlist.at(i)->setState(ChannelIndicatorButton::Default);
    }


    for(int i=1;i<=TOTALCHANNEL;i++)
    {
        qDebug() << i << "B:" <<  GlobalCommon::GetInstance().ChannelIsBrgAvailable(i) << GlobalCommon::GetInstance().bridgeSelfTVal(i)
                 << "I:" << GlobalCommon::GetInstance().ChannelIsInsAvailable(i) << GlobalCommon::GetInstance().insSelfTVal(i)/1000;
    }

    if(0 == type){
        GlobalCommon::GetInstance().saveResultFile(BackWorkThread::CHSELFTEST);
    }
    else{
        GlobalCommon::GetInstance().saveResultFile(BackWorkThread::CHSELFTESTINS);
    }

}

void ChannelTest::settingTestValue(int type)
{
    GlobalCommon::GetInstance().settingSelfTest(type);
}

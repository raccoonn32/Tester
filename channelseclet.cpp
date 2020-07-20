#include "channelseclet.h"
#include "ui_channelseclet.h"
#include "globalcommon.h"
#include "qapphelper.h"

#include "common/confirmw.h"

ChannelSeclet::ChannelSeclet(const QString title, int id, QWidget *parent) :
    cChildQWidget(title,id,parent),
    ui(new Ui::ChannelSeclet)
{
    ui->setupUi(this);
    QFont font = ui->lb_value1->font();
    font.setPixelSize(16);
    ui->lb_value1->setFont(font);
    ui->lb_value2->setFont(font);
    pLayout = new QGridLayout(ui->ChannelMap);
    for(int i=0;i<TOTALCHANNEL;i++)
    {
        ChannelIndicatorButton *tbutton = new ChannelIndicatorButton
                (ChannelIndicatorButton::Button,i+1,(ChannelIndicatorButton::Default),ui->ChannelMap);
        tbutton->setFixedSize(50,50);
        m_channelButtonlist.append(tbutton);
        pLayout->addWidget(tbutton,i/10,i%10);

        tbutton->setFocusProxy(this);//屏蔽 tbuuton的 focus 转交给this处理
        connect(tbutton,SIGNAL(clicked(bool)),this,SLOT(onChannelBtnClicked(bool)));
    }

    ui->ChannelMap->setLayout(pLayout);

    //配置键盘事件的绑定
    {
        connect(this,SIGNAL(newInput(QChar)),this,SLOT(onNewInput(QChar)));
        connect(this,SIGNAL(funcKeyPress(eFuncKeyCode)),this,SLOT(onFuncKeyPress(eFuncKeyCode)));
        connect(this,SIGNAL(cursorKeyPress(eFuncKeyCode)),this,SLOT(onCursorKeyPress(eFuncKeyCode)));
    }

    m_channelType = 0;
}

ChannelSeclet::~ChannelSeclet()
{
    delete ui;
}

//用于设置通道状态
void ChannelSeclet::setChannelsState(QVector<ChannelIndicatorButton::estate> sts)
{
    if(sts.size() != TOTALCHANNEL)
    {
        Q_ASSERT(sts.size() == TOTALCHANNEL);
        return;
    }
    for(int i=0;i< sts.size();i++)
    {
        //此处的i 为通道号 1~30  对应0~29
        m_channelButtonlist.at(i)->setState(sts.at(i));
    }
    //统计选中的总数
    m_selectedTotal = sts.count(ChannelIndicatorButton::Selected);
    emit appStatesString(QString(tr("共选中%1路通道").arg(m_selectedTotal)));
}

//用于设置通道状态
void ChannelSeclet::reflashStates(int type)
{
    if(0 == m_channelType){
        for(int i=1;i<= TOTALCHANNEL;i++)
        {
            //此处的i 为通道号 1~30
            bool state = GlobalCommon::GetInstance().ChannelIsBrgAvailable(i);
            ChannelIndicatorButton::estate st = state ? ChannelIndicatorButton::Selected
                                                      :ChannelIndicatorButton::Unavailable;
            m_channelButtonlist.at(i-1)->setState(st);
        }
    }
    else if(1 == m_channelType){
        for(int i=1;i<= TOTALCHANNEL;i++)
        {
            //此处的i 为通道号 1~30
            bool state = GlobalCommon::GetInstance().ChannelIsInsAvailable(i);
            ChannelIndicatorButton::estate st = state ? ChannelIndicatorButton::Selected
                                                      :ChannelIndicatorButton::Unavailable;
            m_channelButtonlist.at(i-1)->setState(st);
        }
    }

}

void ChannelSeclet::onActivePage(int id)
{
    cChildQWidget::onActivePage(id);
}

//统计选中的总数
void ChannelSeclet::sumupSelected()
{
    int total = 0;
    for(int i=0;i < m_channelButtonlist.size(); i++)
    {
        if(m_channelButtonlist.at(i)->state() ==  ChannelIndicatorButton::Selected)
            total ++;
    }
    m_selectedTotal = total;

    if(m_channelType == 0)
        emit appStatesString(QString(tr("共选中%1路桥路通道").arg(m_selectedTotal)));
    if(m_channelType == 1)
        emit appStatesString(QString(tr("共选中%1路绝缘通道").arg(m_selectedTotal)));
}

//用于设置通道状态 此处的id 为通道号 1开始  设置为选中 不可用 或者 不选中
void ChannelSeclet::setChannelsState(int id, ChannelIndicatorButton::estate st)
{
    int pos = id-1;
    if(pos < 0 || pos > m_channelButtonlist.size())
        return;
    m_channelButtonlist.at(pos)->setState(st);

    //统计选中的总数
    sumupSelected();
}

void ChannelSeclet::onNewInput(QChar c)
{
    QString tmp = m_keyboardinput;
    tmp.append(c);
    if(tmp.toInt() <1 || tmp.toInt() > TOTALCHANNEL)
        m_keyboardinput.clear();
    else
        m_keyboardinput.append(c);
    updateKeyBoardInput();
}

void ChannelSeclet::onFuncKeyPress(ChannelSeclet::eFuncKeyCode c)
{
    if(c == Enter)
    {
        //按下回车 将输入的通道选中 或者 选中光标所指向的（框选的）通道
        if(m_keyboardinput.isEmpty())
            setChannelsState(m_currentFocusButtonId,ChannelIndicatorButton::Selected);
        else
        {
            setChannelsState(m_keyboardpresetid,ChannelIndicatorButton::Selected);
        }
        m_keyboardinput.clear();
    }else if(c == BackSpace)
    {
        if(!m_keyboardinput.isEmpty())
        {
            m_keyboardinput.resize(m_keyboardinput.size()-1);
        }
    }else if(c == Esc)
    {
        //恢复默认值
        //取消键盘输入的值
        if(m_keyboardinput.isEmpty())
            setChannelsState(m_currentFocusButtonId,ChannelIndicatorButton::UnSelected);
        else
            setChannelsState(m_keyboardpresetid,ChannelIndicatorButton::UnSelected);
        m_keyboardinput.clear();
    }else{
        return;
    }
    updateKeyBoardInput();
}

void ChannelSeclet::onCursorKeyPress(ChannelSeclet::eFuncKeyCode c)
{
    m_currentFocusButtonId -=1;
    if(c == Left)
    {
        m_currentFocusButtonId =  (m_currentFocusButtonId % 10 == 0) ? m_currentFocusButtonId+9 : m_currentFocusButtonId-1;
    }else if(Right == c)
    {
        m_currentFocusButtonId =  (m_currentFocusButtonId % 10 == 9) ? m_currentFocusButtonId-9 : m_currentFocusButtonId+1;
    }else if(Up == c)
    {
        m_currentFocusButtonId =  (m_currentFocusButtonId / 10 == 0) ? m_currentFocusButtonId+20 : m_currentFocusButtonId-10;
    }else if(Down == c)
    {
        m_currentFocusButtonId =  (m_currentFocusButtonId / 20 == 1) ? m_currentFocusButtonId-20 : m_currentFocusButtonId+10;
    }
    m_currentFocusButtonId +=1;
    setCurrentChannelId(m_currentFocusButtonId);
}

//鼠标点击的选中
void ChannelSeclet::onChannelBtnClicked(bool)
{
    QObject *sender = QObject::sender();
    QString cName = sender->metaObject()->className();
    if(cName == "ChannelIndicatorButton")
    {
        ChannelIndicatorButton *tb = dynamic_cast<ChannelIndicatorButton *>(sender);
        //更新指向标记
        setCurrentChannelId(tb->id());
        sumupSelected();
    }
}


//传入通道id 标记当前操作的id
void ChannelSeclet::setCurrentChannelId(int id)
{
    if(id < 1 || id>m_channelButtonlist.size())
        return;
    int pos = id - 1;
    m_currentFocusButtonId = id;
    for(int i = 0;i < m_channelButtonlist.size();i++)
    {
        m_channelButtonlist.at(i)->setCurrent(pos == i?true:false);
    }

    float db = GlobalCommon::GetInstance().bridgeSelfTVal(id);
    float di = GlobalCommon::GetInstance().insSelfTVal(id);
    //桥路的默认单位是毫欧  d的单位是欧姆
    //绝缘的默认单位是兆欧  d的单位是欧姆  目前的单位不确定是什么
    float dbb = db/**1000*/;
    float dii = di/1000 /*/1000/1000*/;
    QString l1 = (QString::number(dbb,'f',2)+"Ω");
    QString l2 = (QString::number(dii,'f',2)+"MΩ");

    setLabels(id,l1,l2);
}

void ChannelSeclet::updateKeyBoardInput()
{
    if(!m_keyboardinput.isEmpty())
    {
        ui->lb_keyboardinput->setText(tr("正在输入:")+QString(tr("第%1号通道")).arg(m_keyboardinput));
        m_keyboardpresetid = m_keyboardinput.toInt();
        setCurrentChannelId(m_keyboardpresetid);
    }
    else
    {
        ui->lb_keyboardinput->clear();
        m_keyboardpresetid = 0;
    }
}


void ChannelSeclet::currentDisplay(int id, QString vl1,QString vl2)
{
    setLabels(id,vl1,vl2);
}

void ChannelSeclet::setLabels(int id, QString vl1,QString vl2)
{
    if(m_channelType == 0){
        ui->lb_No->setText(QString(tr("第%1通道桥路自检电阻:  \n(欧)")).arg(id));
        ui->lb_value1->setText(vl1);
        ui->lb_value2->setText("----");
    }
    else if(m_channelType == 1){
        ui->lb_No->setText(QString(tr("第%1通道绝缘自检电阻:  \n(兆欧)")).arg(id));
        ui->lb_value1->setText("----");
        ui->lb_value2->setText(vl2);
    }
}

void ChannelSeclet::executeFunc()
{
    ConfirmW cfw;
    if(m_channelType == 0)
        cfw.setText(QString("\"通道选择\"完成,共选中桥路通道:%1路，确认吗?").arg(m_selectedTotal));
    if(m_channelType == 1)
        cfw.setText(QString("\"通道选择\"完成,共选中绝缘通道:%1路，确认吗?").arg(m_selectedTotal));
    int res = cfw.exec();
    if(res == QDialog::Accepted)
    {
        //通道选中生效
        for(int i=0;i < m_channelButtonlist.size(); i++)
        {
            if(m_channelButtonlist.at(i)->state() ==  ChannelIndicatorButton::Selected)
                GlobalCommon::GetInstance().setChannelSelected(i);
            else
                GlobalCommon::GetInstance().setChannelUnselected(i);
        }
    }
}

void ChannelSeclet::on_allSelect_clicked()
{
    m_selectedTotal = TOTALCHANNEL;

    for(int i=0;i < TOTALCHANNEL; i++)
    {
        m_channelButtonlist.at(i)->setState(ChannelIndicatorButton::Selected);
        GlobalCommon::GetInstance().setChannelSelected(i);
    }
    sumupSelected();    //界面提示选中多少通道
}

void ChannelSeclet::on_noSelect_clicked()
{
    m_selectedTotal = 0;
    for(int i=0;i < TOTALCHANNEL; i++)
    {
        m_channelButtonlist.at(i)->setState(ChannelIndicatorButton::UnSelected);
        GlobalCommon::GetInstance().setChannelUnselected(i);
    }
    sumupSelected();
}

void ChannelSeclet::on_selswitch_clicked()
{    
    for(int i = 0;i < TOTALCHANNEL; i++)
    {
        m_channelButtonlist.at(i)->setState(ChannelIndicatorButton::UnSelected);
    }

    if(ui->selswitch->text() == "桥路通道"){
        m_channelType = 0;
        GlobalCommon::GetInstance().setChannelType(0);
        ui->selswitch->setText("绝缘通道");
    }
    else if(ui->selswitch->text() == "绝缘通道"){
        m_channelType = 1;
        GlobalCommon::GetInstance().setChannelType(1);
        ui->selswitch->setText("桥路通道");
    }

    if(0 == m_channelType){
        for(int i=1;i<= TOTALCHANNEL;i++)
        {
            //此处的i 为通道号 1~30
            bool state = GlobalCommon::GetInstance().ChannelIsBrgAvailable(i);
            ChannelIndicatorButton::estate st = state ? ChannelIndicatorButton::Selected
                                                      :ChannelIndicatorButton::Unavailable;
            m_channelButtonlist.at(i-1)->setState(st);
        }
    }
    else if(1 == m_channelType){
        for(int i=1;i<= TOTALCHANNEL;i++)
        {
            //此处的i 为通道号 1~30
            bool state = GlobalCommon::GetInstance().ChannelIsInsAvailable(i);
            ChannelIndicatorButton::estate st = state ? ChannelIndicatorButton::Selected
                                                      :ChannelIndicatorButton::Unavailable;
            m_channelButtonlist.at(i-1)->setState(st);
        }
    }

}

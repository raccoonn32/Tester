#include "channelindicatorbutton.h"
#include <QDebug>

ChannelIndicatorButton::ChannelIndicatorButton(QWidget *parent) : QAbstractButton(parent)
{
    rcpath = ":/ico/channel/";
    suffix = ".png";
    setMode(Indicator);
    setState(Default);
    setId(-1);
    m_timer.setInterval(300);
    m_timer.setSingleShot(false);
    m_tsting=false;
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
}

ChannelIndicatorButton::ChannelIndicatorButton(ChannelIndicatorButton::emode mode, int id, ChannelIndicatorButton::estate st, QWidget *parent)
    : QAbstractButton(parent)
{
    rcpath = ":/ico/channel/";
    suffix = ".png";
    setMode(mode);
    setState(st);
    setId(id);
    setCurrent(false);
    m_timer.setInterval(300);
    m_timer.setSingleShot(false);
    m_tsting=false;
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
}

void ChannelIndicatorButton::setState(estate st)
{
    m_states = st;
    if(m_states == Testing)
    {
        m_timer.start();
    }else
    {
        m_timer.stop();m_tsting = false;
    }

    if(m_states == Unavailable)
    {
        setMode(Indicator);
    }else if(m_states == Selected)
    {
        setChecked(true);
        setMode(Button);
    }else if(m_states == UnSelected)
    {
        setChecked(false);
        setMode(Button);
    }
    update();
}

ChannelIndicatorButton::estate ChannelIndicatorButton::state()
{
    return m_states;
}

void ChannelIndicatorButton::setMode(ChannelIndicatorButton::emode md)
{
    m_mode = md;
    //指示灯模式
    if(md == Indicator)
    {
        setEnabled(false);
        setCheckable(false);
        setChecked(false);
        disconnect(this,SIGNAL(toggled(bool)),this,SLOT(onCheckedChange(bool)));
    }else
        //button模式
    {
        setEnabled(true);
        setCheckable(true);
        connect(this,SIGNAL(toggled(bool)),this,SLOT(onCheckedChange(bool)));
    }
}

void ChannelIndicatorButton::setId(int id)
{
    m_id = id;
}

int ChannelIndicatorButton::id()
{
    return m_id;
}

void ChannelIndicatorButton::setCurrent(bool c)
{
    m_isCurrent = c;
    update();
}

bool ChannelIndicatorButton::isCurrent()
{
    return m_isCurrent;
}


void ChannelIndicatorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    QString imgpath;

    if(m_states == Unavailable)
    {
        imgpath.append("red");
    }else if(m_mode == Indicator)
    {
        if(m_states == Available)
        {
            imgpath.append("green");
        }else if(m_states == Testing)
        {
            imgpath.append(m_tsting?"white":"yellow");
        }else
        {
            imgpath.append("white");
        }
    }else if(m_mode == Button)//button模式的绘制条件不同
    {
        if(isChecked() == true)
        {
            imgpath.append("green");
        }else
        {
            imgpath.append("white");
        }
    }

    if(!imgpath.isEmpty())
        painter.drawPixmap(this->rect(),QPixmap(rcpath+imgpath+suffix));

    if (m_states == Unavailable)
        painter.drawPixmap(this->rect(),QPixmap(rcpath+"x"+suffix));

    if(m_mode==Button & m_isCurrent)
    {
        painter.setPen(Qt::red);
        painter.drawRect(0,0,this->rect().width()-1,this->rect().height()-1);
    }

    if(m_id >0)
    {
        QPen pen;
        pen.setColor(Qt::black);
        QFont font;
        font.setBold(true);
        float size = qMin(this->rect().width(),this->rect().height()) *0.6;
        font.setPixelSize(size);
        painter.setPen(pen);
        painter.setFont(font);
        QTextOption textOption;
        textOption.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        painter.drawText(this->rect(),QString::number(m_id),textOption);
    }
}

//checked 信号绑定状态 select 或者 unselected
void ChannelIndicatorButton::onCheckedChange(bool ck)
{
    if(m_mode != Indicator)
        setState(ck?Selected:UnSelected);
}

void ChannelIndicatorButton::onTimeOut()
{
    m_tsting = !m_tsting;
    update();
}

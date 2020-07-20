#include "qled.h"

QLed::QLed(QWidget *parent) : QWidget(parent)
{
    m_value=false;
    m_onColor=Red;
    m_offColor=Grey;

    rcpath = ":/ico/circle_";
    suffix = ".png";
    colors << "black" << "blue" << "green" << "grey"
           << "orange" << "purple" << "yellow"  << "red";

}

QLed::~QLed()
{

}

void QLed::setValue(bool value)
{
    m_value=value;
    update();
}

void QLed::setOnColor(QLed::ledColor newColor)
{
    m_onColor=newColor;
    update();
}

void QLed::setOffColor(QLed::ledColor newColor)
{
    m_offColor=newColor;
    update();
}

void QLed::toggleValue()
{
    m_value =!m_value;
    update();
}

void QLed::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QString rc;
    rc.append(rcpath);
    if(m_value){
        rc.append(colors[m_onColor]);
    }
    else {
        rc.append(colors[m_offColor]);
    }
    rc.append(suffix);

    painter.drawPixmap(this->rect(),QPixmap(rc));
}

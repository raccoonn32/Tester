#ifndef CHANNELINDICATORBUTTON_H
#define CHANNELINDICATORBUTTON_H

#include <QWidget>
#include <QAbstractButton>
#include <QPainter>

#include <QTimer>

class ChannelIndicatorButton : public QAbstractButton
{
    Q_OBJECT
public:
    //包含了 两种工作模式下的状态
    enum estate {Default=0,Available,Testing,
                 Unavailable,Selected ,UnSelected};

    //工作模式 指示 按钮
    enum emode {Indicator=0,Button};

    ChannelIndicatorButton(QWidget *parent = NULL);

    ChannelIndicatorButton(emode mode,int id = -1, estate st = Default,QWidget *parent = NULL);

    void setState(estate st);
    estate state();
    void setMode(emode md);
    void setId(int id);
    int id();

    void setCurrent(bool c=true);
    bool isCurrent();
signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void onCheckedChange(bool ck);

    void onTimeOut();
private:
    estate m_states; //通道状态
    emode m_mode;// 控件模式
    int m_id;//通道号
    QString rcpath;
    QString suffix;
    bool m_isCurrent;//标记是否当前光标指向
    QTimer m_timer;
    bool m_tsting;//
};

#endif // CHANNELINDICATORBUTTON_H

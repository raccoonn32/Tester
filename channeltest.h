#ifndef CHANNELTEST_H
#define CHANNELTEST_H

#include <QWidget>
#include <QGridLayout>
#include <QList>

#include "channelindicatorbutton.h"
#include "cchildqwidget.h"
#include "globalcommon.h"

namespace Ui {
class ChannelTest;
}

class ChannelTest : public cChildQWidget
{
    Q_OBJECT

public:
    explicit ChannelTest(const QString title, int id, QWidget *parent = NULL);
    ~ChannelTest();

    void preExecute(); //按下当前页标签执行的内容
    void executeFunc(int type);
public slots:
    void currentDisplay(int id, QString vl1,QString vl2);

    void currentTest(int current);
    void onchannelSeflTestRes(int type,int channel,bool b,float dat);

    void onSelfTestOver(int type);//所有通道自检完成

    void settingTestValue(int type); //保存自检的电阻值
private:
    void setLabels(int id, QString vl1, QString vl2);

private:
    Ui::ChannelTest *ui;
    QGridLayout *pLayout;
    QList<ChannelIndicatorButton *> m_channelIndicatorlist;
};

#endif // CHANNELTEST_H

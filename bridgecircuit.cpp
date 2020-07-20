#include "bridgecircuit.h"

#include "qapphelper.h"
#include "backworkthread.h"
#include "globalcommon.h"

#include "common/confirmw.h"

BridgeCircuit::BridgeCircuit(const QString title, int id, QWidget *parent) :
    BaseTableWidget(title,id,parent)
{
    setUnitLabel(tr("单位:欧姆"));
}

BridgeCircuit::~BridgeCircuit()
{

}


void BridgeCircuit::onActivePage(int id)
{
    cChildQWidget::onActivePage(id);
    //选中的未设置的通道设置未空
    resetItemText("---");
    QVector<int> channelList = GlobalCommon::GetInstance().selectedChannelList();
    foreach (int current, channelList) {
        setItemText(current,"");
    }
}

void BridgeCircuit::onCurrentChannelChanged(int id, QString vl)
{
    //id通道号
    setRightLabel(QString(tr("第%1通道桥路电阻：\n(欧姆)")).arg(id));
    setRightValue(vl);
}

//通道号  是否可信  数据
void BridgeCircuit::onCurrentChannelChanged(int channel, bool b,float dat)
{
    //id通道号
    //setRightLabel(QString(tr("第%1通道桥路电阻：\n(欧姆)")).arg(channel));
    if(b)
    {
        onCurrentChannelChanged(channel);
        //        //桥路的默认单位是毫欧  d的单位是欧姆
        //        float d = dat*1000;
        //        QString l1 = (QString::number(d,'f',2) +"mΩ");
        //        setRightValue(l1);
        //    }else{
        //        setRightValue("---");
    }
}

void BridgeCircuit::onCurrentChannelChanged(int id)
{
    bool res = GlobalCommon::GetInstance().bridgeValReliable(id);
    qDebug() << __FUNCTION__ << id << res;
    if(res)
    {
        double dat = GlobalCommon::GetInstance().bridgeVal(id);
        setItemText(id-1,QString::number(dat,'f',2)+"Ω");
    }else
    {
        setItemText(id-1,"---");
    }
    setRightLabel(QString(tr("第%1通道桥路电阻：\n(欧姆)")).arg(id));
    QString str(itemText(id-1));
    setRightValue(str);
}

void BridgeCircuit::executeFunc()
{
    ConfirmW cfw;
    cfw.setText("系统将执行\"桥路电阻测量\",确认吗?");
    //cfw.setGPIOButton(GPIO(1,16),GPIO(1,17));
    int res = cfw.exec();
    if(res == QDialog::Accepted)
        GlobalCommon::GetInstance().startWork(BackWorkThread::BRITEST);
}

void BridgeCircuit::onTestOver()
{
    emit appStatesString(QString(tr("桥路电阻测试完毕")));
    GlobalCommon::GetInstance().saveResultFile(BackWorkThread::BRITEST);
}

void BridgeCircuit::onCurrentTest(int i)
{
    qDebug() <<__PRETTY_FUNCTION__ <<i;
    emit appStatesString(QString(tr("正在测量第%1路").arg(i)));
}

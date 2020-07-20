#include "insulation.h"

#include "common/confirmw.h"

#include "voltageset.h"

Insulation::Insulation(const QString title, int id, QWidget *parent) :
    BaseTableWidget(title,id,parent)
{
    setLeftLabel(tr("绝缘测试电压:"));
    setUnitLabel(tr("单位:兆欧"));
}

Insulation::~Insulation()
{

}

void Insulation::onActivePage(int id)
{
    cChildQWidget::onActivePage(id);
    quint8 vol = GlobalCommon::GetInstance().insulationVoltage();
    onInsulationVoltageChanged(vol);
    //选中的未设置的通道设置未空
    resetItemText("---");
    QVector<int> channelList = GlobalCommon::GetInstance().selectedChannelList();
    foreach (int current, channelList) {
        setItemText(current,"");
    }
}

void Insulation::executeFunc()
{
    ConfirmW cfw;
    cfw.setText("系统将执行\"绝缘电阻测量\",确认吗?");
    //cfw.setGPIOButton(GPIO(7,2),GPIO(7,3));
    int res = cfw.exec();
    if(res == QDialog::Accepted)
        GlobalCommon::GetInstance().startWork(BackWorkThread::INSTEST);
}

void Insulation::onInsulationVoltageChanged(quint8 vol)
{
    int vol_ = VoltageSet::bcd2decimal(vol);

    qDebug() << vol << QString::number(vol,16) << vol_;
    vol_ *=10;
    setLeftValue(QString::number(vol_)+'V');
}

void Insulation::onCurrentChannelChanged(int id,QString vl)
{
    //id通道号
    setRightLabel(QString(tr("第%1通道绝缘电阻：\n(兆欧)")).arg(id));
    setRightValue(vl);
}

void Insulation::onCurrentChannelChanged(int id, bool b, float dat)
{
    if(b)
    {
        onCurrentChannelChanged(id);
    }
}


void Insulation::onCurrentChannelChanged(int id)
{
    bool res = GlobalCommon::GetInstance().insValReliable(id);
    if(res)
    {
        double dat = GlobalCommon::GetInstance().insVal(id);
        setItemText(id-1,QString::number(dat,'f',2)+"MΩ");
    }else
    {
        setItemText(id-1,"---");
    }
    setRightLabel(QString(tr("第%1通道绝缘电阻：\n(兆欧)")).arg(id));
    QString str(itemText(id-1));
    setRightValue(str);
}

void Insulation::onTestOver()
{
    emit appStatesString(QString(tr("绝缘电阻测试完毕")));
    GlobalCommon::GetInstance().saveResultFile(BackWorkThread::INSTEST);
}


void Insulation::onCurrentTest(int i)
{
    emit appStatesString(QString(tr("正在测量第%1路").arg(i)));
}

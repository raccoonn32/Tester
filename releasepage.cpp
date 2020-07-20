#include "releasepage.h"
#include "globalcommon.h"

ReleasePage::ReleasePage(const QString title, int id, QWidget *parent) :
    BaseTableWidget(title,id,parent)
{
    setUnitLabel(tr("单位:兆欧"));
}

ReleasePage::~ReleasePage()
{

}

void ReleasePage::onActivePage(int id)
{
    cChildQWidget::onActivePage(id);
    //选中的未设置的通道设置未空
    resetItemText("---");
    QVector<int> channelList = GlobalCommon::GetInstance().selectedChannelList();
    foreach (int current, channelList) {
        setItemText(current,"");
    }
}

void ReleasePage::onCurrentChannelChanged(int id,QString vl)
{
    //id通道号
    setRightLabel(QString(tr("第%1通道泄放电阻：\n(兆欧)")).arg(id));
    setRightValue(vl);
}

void ReleasePage::onCurrentChannelChanged(int id, bool b, float dat)
{
    if(b)
    {
        onCurrentChannelChanged(id);
    }
}

void ReleasePage::onCurrentChannelChanged(int id)
{
    bool res = GlobalCommon::GetInstance().releaseValReliable(id);
    if(res)
    {
        double dat = GlobalCommon::GetInstance().releaseVal(id);
        setItemText(id-1,QString::number(dat,'f',2)+"MΩ");
    }else
    {
        setItemText(id-1,"---");
    }
    setRightLabel(QString(tr("第%1通道泄放电阻：\n(兆欧)")).arg(id));
    QString str(item(id-1).text());
    setRightValue(str);
}

void ReleasePage::executeFunc()
{
    ConfirmW cfw;
    cfw.setText("系统将执行\"泄放程序\",确认吗?");
    int res = cfw.exec();
    if(res == QDialog::Accepted)
        GlobalCommon::GetInstance().startWork(BackWorkThread::RELTEST);
}

void ReleasePage::onTestOver()
{
    emit appStatesString(QString(tr("泄放测试完毕")));
    GlobalCommon::GetInstance().saveResultFile(BackWorkThread::RELTEST);
}

void ReleasePage::onCurrentTest(int i)
{
    emit appStatesString(QString(tr("正在泄放第%1路").arg(i)));
}

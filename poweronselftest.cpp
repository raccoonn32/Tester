#include "poweronselftest.h"
#include "ui_poweronselftest.h"

PowerOnSelfTest::PowerOnSelfTest(const QString title, int id, QWidget *parent) :
    cChildQWidget(title,id,parent),
    ui(new Ui::PowerOnSelfTest)
{
    ui->setupUi(this);

    ui->ledPass->setOnColor(QLed::Green);
    ui->ledFail->setOnColor(QLed::Red);

    ui->ledPass->setValue(false);
    ui->ledFail->setValue(false);
}

PowerOnSelfTest::~PowerOnSelfTest()
{
    delete ui;
}

void PowerOnSelfTest::onPwSelfTestResultChanged()
{
    GlobalCommon::SelfTestResult cres = GlobalCommon::GetInstance().controllerSelfTestResult();
    GlobalCommon::SelfTestResult dres = GlobalCommon::GetInstance().detectorSelfTestResult();
    if(cres ==  GlobalCommon::controllerOk )
    {
        ui->lb_controllerResult->setText(QString(tr("正常")));
    }else if(cres ==  GlobalCommon::controllerFailur)
    {
        ui->lb_controllerResult->setText(QString(tr("失败")));
        setStateStr(QString(tr("自检失败")));
    }else if(cres == GlobalCommon::untest)
    {
        ui->lb_controllerResult->setText(QString(tr("待检测")));
    }

    if(dres ==  GlobalCommon::detectorOk )
    {
        ui->lb_detectorResult->setText(QString(tr("正常")));
    }else if(dres ==  GlobalCommon::detectorFailur)
    {
        ui->lb_detectorResult->setText(QString(tr("失败")));
        setStateStr(QString(tr("自检失败")));
    }else if(dres == GlobalCommon::untest)
    {
        ui->lb_detectorResult->setText(QString(tr("待检测")));
    }

    if(dres ==  GlobalCommon::detectorOk &&  cres ==  GlobalCommon::controllerOk )
    {
        ui->ledPass->setValue(true);
        ui->ledFail->setValue(false);
        setStateStr(QString(tr("自检成功")));
    }else
    {
        ui->ledPass->setValue(false);
        ui->ledFail->setValue(true);
    }
}



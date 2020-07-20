#include "cutoffpage.h"
#include "ui_cutoffpage.h"

#include "qapphelper.h"
#include "common/confirmw.h"

#define CODebug()  qDebug() << __FILE__ << __LINE__

CutOffPage::CutOffPage(QWidget *parent) :
    cChildQWidget(parent),
    ui(new Ui::CutOffPage)
{
    ui->setupUi(this);
}

CutOffPage::CutOffPage(const QString title, int id, QWidget *parent) :
    cChildQWidget(title,id,parent),
    ui(new Ui::CutOffPage)
{
    ui->setupUi(this);

    setCutOffState(Unexecuted);//设置默认状态
}

CutOffPage::~CutOffPage()
{
    delete ui;
}

//设置断开状态
void CutOffPage::setCutOffState(CutOffPage::eCutOffSta st)
{
    if(st == Unexecuted) //尚未执行断开通道功能
    {
        ui->lb_cutstring->setText("警告!将切断全部通道!");
    }else if(st == Disconnected)
    {
        ui->lb_cutstring->setText("所有通道已切断!");
    }
}

void CutOffPage::executeFunc()
{
    ConfirmW cfw;
    cfw.setText("系统将切断所有通道,确认吗?");
    int res = cfw.exec();
    if(res == QDialog::Accepted)
        GlobalCommon::GetInstance().startWork(BackWorkThread::CUTOFF);
}

void CutOffPage::onCutOffOver()
{
    emit appStatesString(QString(tr("发出切断通道指令")));

    ui->lb_cutstring->setText("所有通道已切断！");
}

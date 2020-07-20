#include "selftestdialog.h"
#include "ui_selftestdialog.h"

#include "qapphelper.h"
#include "globalcommon.h"

SelfTestDialog::SelfTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelfTestDialog)
{
    ui->setupUi(this);

    //modbus 后端的初始化  额外线程
    m_mb232 = new modbus_backend(MB_232_UART,9600);

    //m_timerout 超时计时器
    m_timerout = new QTimer(this);
    m_timerout->setInterval(1000);
    m_timerout->setSingleShot(false);
    connect(m_timerout,SIGNAL(timeout()),this,SLOT(onTimeTick()));
    m_remaintime = SELFTEST_TIMEOUT;
    ui->progressBar->setMaximum(SELFTEST_TIMEOUT);
    onTimeTick();//初次初始化

    //test:
    //m_mb232->start();

}

SelfTestDialog::~SelfTestDialog()
{
    disconnect(this);
    delete ui;
}

void SelfTestDialog::closeEvent(QCloseEvent *event)
{
    qDebug() << __FUNCTION__;
    m_mb232->stop();
    m_mb232->wait();
    disconnect(m_mb232);
    delete m_mb232;
    disconnect(m_timerout);
    delete m_timerout;
}

//自检
void SelfTestDialog::startTest()
{
    //开始自检
    selfTestStart();
}

void SelfTestDialog::on_pb_cancel_clicked()
{
    this->close();
}

// 秒tick 事件
void SelfTestDialog::onTimeTick()
{
    if(m_remaintime > 0)
    {
        m_remaintime--;
    }else{
        m_timerout->stop();
        selfTestStep1();
    }
    ui->progressBar->setValue(SELFTEST_TIMEOUT - m_remaintime);
    ui->lb_remainTime->setText(QString::number(m_remaintime)+'S');
}

//自检过程 开始自检
void SelfTestDialog::selfTestStart()
{
    //发送自检命令 400100 127
    int addr = 40010;
    int value = 127;
    m_mb232->writeRegister(addr,value);
    //开始定时
    m_timerout->start();
    QTimer t;
    QEventLoop q;
    connect(&t,SIGNAL(timeout()),&q,SLOT(quit()));
    t.start(5000);
    qDebug() << "evenloop test start";
    q.exec();
    qDebug() << "evenloop test end";
}

//自检过程 步骤1  超时后的过程
void SelfTestDialog::selfTestStep1()
{
    //读取寄存器 400200
    int res;

    qDebug() << "self Test value :" << res;
    if(res == 7)
    {
        QAppHelper::ShowMessageBoxInfo(tr("Self Test pass!"));
    //    GlobalCommon::GetInstance().setSelfTestState(true);
        this->close();
        return;
    }
    QAppHelper::ShowMessageBoxError(tr("Self Test failue!"));
   // GlobalCommon::GetInstance().setSelfTestState(false);
    this->close();
}

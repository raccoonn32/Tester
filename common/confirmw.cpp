#include "confirmw.h"
#include "ui_confirmw.h"

#include <QDesktopWidget>
#include <QDialog>
ConfirmW::ConfirmW(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfirmW)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    connect(ui->pbCancel,SIGNAL(clicked(bool)),this,SLOT(onCancelClicked(bool)));
    connect(ui->pbOk,SIGNAL(clicked(bool)),this,SLOT(onOkClicked(bool)));
    ui->pbOk->setShortcut(Qt::Key_Enter);
    ui->pbOk->setShortcut(Qt::Key_Return);
    ui->pbCancel->setShortcut(Qt::Key_Escape);
}

ConfirmW::~ConfirmW()
{
    delete ui;
}

int ConfirmW::exec()
{
    // 设置为模态;
    this->setWindowModality(Qt::ApplicationModal);
    show();
    move ((QApplication::desktop()->width() - width())/2,(QApplication::desktop()->height() - height())/2);
    // 使用事件循环QEventLoop ,不让exec()方法结束，在用户选择确定或者取消后，关闭窗口结束事件循环，并返回最后用户选择的结果;
    // 根据返回结果得到用户按下了确定还是取消，采取相应的操作。从而模拟出QDialog类的exec()方法;
    m_eventLoop = new QEventLoop(this);
    m_eventLoop->exec();

    return m_chooseResult;
}

void ConfirmW::setText(const QString &s)
{
    ui->text->setText(s);
}

void ConfirmW::setGPIOButton(int pin1, int pin2)
{
#if !defined(Q_OS_WIN32)
    pinOk = pin1;
    pinCancel = pin2;
    m_gpiopin.setGpio(pinOk,false,false);
    m_gpiopin.setGpio(pinCancel,false,false);

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);// read gpio button interval
    m_timer->setSingleShot(true);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(onGpioInput()));
    m_timer->start();
#endif
}

void ConfirmW::onOkClicked(bool v)
{
    m_chooseResult = QDialog::Accepted;
    close();
}

void ConfirmW::onCancelClicked(bool v)
{
    m_chooseResult = QDialog::Rejected;
    close();
}

void ConfirmW::onGpioInput()
{
    bool res = m_gpiopin.getGpio(pinCancel,false);    
    //qDebug() << __FUNCTION__ << res;
    if(res) onCancelClicked(false);
    res = m_gpiopin.getGpio(pinOk,false);    
    //qDebug() << __FUNCTION__ << res;
    if(res) onOkClicked(false);

    if(m_timer) m_timer->start();
}

void ConfirmW::closeEvent(QCloseEvent *event)
{
    // 关闭窗口时结束事件循环，在exec()方法中返回选择结果;
    if (m_eventLoop != NULL)
    {
        m_eventLoop->exit();
    }
    event->accept();
}

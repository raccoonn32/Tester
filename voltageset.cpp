#include "voltageset.h"
#include "ui_voltageset.h"

#include "qapphelper.h"
#include "common/confirmw.h"


//注意该方法 转换范围  不适用于所有情况
unsigned char VoltageSet::bcd2decimal(unsigned char bcd)
{
    return bcd-(bcd >> 4)*6;
}

//注意该方法 转换范围  不适用于所有情况
 unsigned char VoltageSet::decimal2bcd(unsigned char decimal)
{
    return (unsigned char) (decimal+(decimal / 10)*6);
}

VoltageSet::VoltageSet(const QString title, int id, QWidget *parent) :
    cChildQWidget(title,id,parent),
    ui(new Ui::VoltageSet)
{
    ui->setupUi(this);
    //预设电压列表
    voltageList << 500 << 250 << 100;

    //捕捉键盘事件
#if 0
    this->installEventFilter(this);
    connect(this,SIGNAL(newInput(QChar)),this,SLOT(onNewInput(QChar)));
    connect(this,SIGNAL(funcKeyPress(eFuncKeyCode)),this,SLOT(onFuncKeyPress(eFuncKeyCode)));
#endif
     GlobalCommon::GetInstance().setInsulationVoltage(0X50); //默认值为50
    setPreVoltage();
}

VoltageSet::~VoltageSet()
{
    delete ui;
}

void VoltageSet::executeFunc()
{
    ConfirmW cfw;
    cfw.setText(QString("系统要将绝缘测试电压设为:%1V DC,确认吗?").arg(m_preVoltage));
    int res = cfw.exec();
    if(res == QDialog::Accepted)
    {
        //由于绝缘电压 用的类似BCD编码 的表示方式   0x50 表示 DEC（50）  表示500V
        quint8 p = decimal2bcd((unsigned char)(m_preVoltage/10));

        //BCD
        GlobalCommon::GetInstance().setInsulationVoltage(p);
        setStateStr(tr("设置绝缘测试电压为%1VDC").arg(m_preVoltage));
        qDebug() << QString("设置绝缘测试电压为%1VDC").arg(m_preVoltage) << " BCD "<< QString::number(p,16);
    }
}



//用于切换预设电压
void VoltageSet::setPreVoltage()
{
    static quint8 conut = 0;
    setPreVoltage(voltageList.at(conut%3));
    conut++;
}

void VoltageSet::onEnterButtonClick(bool ck)
{
    if(isActive())
    {
        setPreVoltage();
        setFocus();
    }
}


void VoltageSet::onNewInput(QChar c)
{
    m_keyboardinput.append(c);
    updateKeyBoardInput();
}

void VoltageSet::onFuncKeyPress(eFuncKeyCode c)
{
    if(c == Enter)
    {
        //按下回车 将输入的电压设置到 主界面
        setPreVoltage(m_keyboardinput.toInt());
    }else if(c == BackSpace)
    {
        if(!m_keyboardinput.isEmpty())
        {
            m_keyboardinput.resize(m_keyboardinput.size()-1);
        }
    }else if(c == Esc)
    {
        //恢复默认值
        //取消键盘输入的值
        m_keyboardinput.clear();
        setPreVoltage(voltageList.at(0));
    }
    updateKeyBoardInput();
}

void VoltageSet::updateKeyBoardInput()
{
    if(!m_keyboardinput.isEmpty())
        ui->lb_keyboardinput->setText(tr("正在输入:")+m_keyboardinput+tr(" VDC"));
    else
        ui->lb_keyboardinput->clear();
}

void VoltageSet::setPreVoltage(int v)
{
    m_preVoltage = v;
    ui->lb_preVoltage->setText(QString::number(m_preVoltage)+" VDC");
}


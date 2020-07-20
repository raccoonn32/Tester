#include "basetable.h"
#include "ui_basetable.h"

#include "qapphelper.h"
#include "globalcommon.h"


BaseTableWidget::BaseTableWidget(const QString title, int id, QWidget *parent) :
    cChildQWidget(title,id,parent),
    ui(new Ui::BaseTableWidget)
{
    ui->setupUi(this);

    initTable();
    QFont font = ui->lb_right_value->font();
    font.setPixelSize(20);
    ui->lb_right_value->setFont(font);
    ui->lb_right->setFont(font);

    ui->lb_left->clear();
    ui->lb_left_value->clear();
    ui->lb_right->clear();
    ui->lb_right_value->clear();

    //键盘事件
    {
        connect(this,SIGNAL(newInput(QChar)),this,SLOT(onNewInput(QChar)));
        connect(this,SIGNAL(funcKeyPress(eFuncKeyCode)),this,SLOT(onFuncKeyPress(eFuncKeyCode)));
        connect(this,SIGNAL(cursorKeyPress(eFuncKeyCode)),this,SLOT(onCursorKeyPress(eFuncKeyCode)));
    }
}

BaseTableWidget::~BaseTableWidget()
{
    delete ui;
}

void BaseTableWidget::initTable()
{
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setVisible(false);


    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //不可修改
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);//只能选择单个

#if QT_VERSION < 0x050000
    //设置等宽度
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#else
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#endif

    //6行  10列 其中3行做编号标记
    int totalChannel = TOTALCHANNEL;

    int columns = 5; //每行5个
    int rows = totalChannel/columns; //共计 行数
    int sRows = rows*2;//实际行数 每个通道有个标号
    ui->tableWidget->setColumnCount(columns);
    ui->tableWidget->setRowCount(sRows);

    for(int i=0; i< ui->tableWidget->rowCount(); i++)
    {
        ui->tableWidget->setRowHeight(i,40);
    }

    for(int i = 0;i<totalChannel*2;i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
        if(i<totalChannel){
            item->setText("---");
            ui->tableWidget->setItem((i/columns)*2+1,i%columns,item);
            items.append(item);//添加到拥有数据的item添加到列表
        }
        else //后面的用作显示标号
        {
            item->setFlags(Qt::NoItemFlags);//不可选中
            item->setText(QString::number(i-totalChannel+1));
            ui->tableWidget->setItem((i-totalChannel)/columns*2,i%columns,item);

        }
    }

    ui->tableWidget->setFocus(Qt::NoFocusReason);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);//无法选中表格中的item
}

void BaseTableWidget::updateKeyBoardInput()
{
    if(!m_keyboardinput.isEmpty())
    {
        ui->lb_keyboardinput->setText(tr("正在输入:")+QString(tr("第%1号通道")).arg(m_keyboardinput));
        //        m_keyboardpresetid = m_keyboardinput.toInt();
        //        setCurrentChannelId(m_keyboardpresetid);
    }
    else
    {
        ui->lb_keyboardinput->clear();
        //  m_keyboardpresetid = 0;
    }
}

void BaseTableWidget::onNewInput(QChar c)
{
    QString tmp = m_keyboardinput;
    tmp.append(c);
    if(tmp.toInt() <1 || tmp.toInt() > 4)
        m_keyboardinput.clear();
    else
        m_keyboardinput.append(c);
    updateKeyBoardInput();
}

void BaseTableWidget::onFuncKeyPress(cChildQWidget::eFuncKeyCode c)
{
    if(c == Enter)
    {
        //按下回车 查询当前输入通道值
        if(m_keyboardinput.isEmpty())
        {
            emit enterKeyPressed();
        }
        m_keyboardinput.clear();
    }else if(c == BackSpace)
    {
        if(!m_keyboardinput.isEmpty())
        {
            m_keyboardinput.resize(m_keyboardinput.size()-1);
        }
    }else if(c == Esc)
    {
        emit escKeyPressed();
    }else{
        return;
    }
    updateKeyBoardInput();
}

void BaseTableWidget::onCursorKeyPress(cChildQWidget::eFuncKeyCode c)
{
    QScrollBar *psb =  ui->tableWidget->verticalScrollBar();
    int pv = psb->value();
    int pmaxv = psb->maximum();
    int pminv = psb->minimum();
    int step = 2;//滚动步数

    if(Up == c)
    {
        pv = (pv-step) > pminv ? pv-step : pminv;
    }else if(Down == c)
    {
        pv = (pv+step) < pmaxv ? pv+step : pmaxv;
    }
    psb->setValue(pv);

}

void BaseTableWidget::setLeftLabel(QString str)
{
    ui->lb_left->setText(str);
}

void BaseTableWidget::setLeftValue(QString str)
{
    ui->lb_left_value->setText(str);
}

void BaseTableWidget::setRightLabel(QString str)
{
    ui->lb_right->setText(str);
}

void BaseTableWidget::setRightValue(QString str)
{
    ui->lb_right_value->setText(str);
}

void BaseTableWidget::setItemText(int id, QString str)
{
    items.at(id)->setText(str);
}

void BaseTableWidget::resetItemText(QString str)
{
    for(int i=0;i< TOTALCHANNEL;i++)
        items.at(i)->setText(str);
}

void BaseTableWidget::setUnitLabel(QString str)
{
    ui->lb_unit->setText(str);
}

QString BaseTableWidget::itemText(int id) const
{
    return items.at(id)->text();
}

const QTableWidgetItem &BaseTableWidget::item(int id) const
{
    return *items.at(id);
}


void BaseTableWidget::on_pushButton_2_clicked()
{
    ui->tableWidget->verticalScrollBar()->setSliderPosition(0);
}

void BaseTableWidget::on_pushButton_clicked()
{
    int maxValue =  ui->tableWidget->verticalScrollBar()->maximum(); // 当前SCROLLER最大显示值25
    ui->tableWidget->verticalScrollBar()->setSliderPosition(maxValue);
}

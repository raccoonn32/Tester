#include "cchildqwidget.h"
#include <QStackedWidget>
#include "confirmw.h"
#include <QDebug>

cChildQWidget::cChildQWidget(QWidget *parent) : QWidget(parent)
{
    m_isActive = false;
    m_id = -1;

    installEventFilter(this);
}

cChildQWidget::cChildQWidget(const QString title, int id, QWidget *parent)
    : QWidget(parent)
{
    m_isActive = false;
    m_title = title;
    m_id = id;
    QString pn = parent->metaObject()->className();
    if(parent != NULL &&  pn == "QStackedWidget")
    {
        QStackedWidget *p = dynamic_cast<QStackedWidget *>(parent);
        connect(p,SIGNAL(currentChanged(int)),this,SLOT(onActivePage(int)));
    }

    installEventFilter(this);
}

cChildQWidget::~cChildQWidget()
{
    disconnect(this);
}

bool cChildQWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this) {
        if (event->type() == QEvent::KeyPress) {

            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
            {
                emit funcKeyPress(Enter);
                return true;
            }else if(keyEvent->key() == Qt::Key_Escape)
            {
                emit funcKeyPress(Esc);
                return true;
            }else if(keyEvent->key() ==  Qt::Key_Backspace)
            {
                emit funcKeyPress(BackSpace);
                return true;
            }else if( keyEvent->key() >= Qt::Key_0 && keyEvent->key() <= Qt::Key_9)
            {
                emit newInput(QChar(keyEvent->text().at(0)));
                return true;
            }else if( keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_W)
            {
                emit cursorKeyPress(Up);
                return true;
            }else if( keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_S)
            {
                emit cursorKeyPress(Down);
                return true;
            }else if( keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_A)
            {
                emit cursorKeyPress(Left);
                return true;
            }else if( keyEvent->key() == Qt::Key_Right || keyEvent->key() == Qt::Key_D)
            {
                emit cursorKeyPress(Right);
                return true;
            }
        } else {
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void cChildQWidget::executeFunc()
{
    qDebug() << __FILE__<< __LINE__ << __FUNCTION__ << m_title << m_stateStr;
}

void cChildQWidget::onActivePage(int id)
{
    if(id == m_id)
    {
        setActive(true);
        appStatesString(stateStr());
        appTitleString(title());
    }else
        setActive(false);
}

int cChildQWidget::id() const
{
    return m_id;
}

void cChildQWidget::setId(const int newValue)
{
    if ( newValue == m_id )
    {
        return;
    }
    m_id = newValue;
}

QString cChildQWidget::title() const
{
    return m_title;
}

QString cChildQWidget::stateStr() const
{
    return m_stateStr;
}

bool cChildQWidget::isActive()
{
    return m_isActive;
}

void cChildQWidget::setTitle(const QString newValue)
{
    if ( newValue == title() )
    {
        return;
    }
    m_title = newValue;
    if(isActive())
        emit appTitleString(m_title);
}

void cChildQWidget::setStateStr(const QString str)
{
    m_stateStr = str;
    if(isActive())
        emit appStatesString(stateStr());
}

void cChildQWidget::setActive(bool act)
{
    m_isActive = act;
    setFocus();
}

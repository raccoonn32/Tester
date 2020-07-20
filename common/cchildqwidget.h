#ifndef CCHILDQWIDGET_H
#define CCHILDQWIDGET_H

#include <QWidget>
#include <QKeyEvent>

class cChildQWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString m_title READ title WRITE setTitle)
    Q_PROPERTY(QString m_stateStr READ stateStr WRITE setStateStr)
    Q_PROPERTY(int m_id READ id WRITE setId)

public:
    explicit cChildQWidget(QWidget *parent = NULL);
    explicit cChildQWidget(const QString title, int id, QWidget *parent = NULL);

    enum eFuncKeyCode{Enter = 1,BackSpace,Esc,Up,Down,Left,Right};

    virtual ~cChildQWidget();
    virtual bool eventFilter(QObject *, QEvent *);

    virtual void executeFunc();//用于外部调用 执行预设功能

signals:
    void appStatesString(const QString str);//用于更新主界面副标题
    void appTitleString(const QString str);//用于更新界面主标题

signals:
    void newInput(QChar c);
    void funcKeyPress(eFuncKeyCode c);
    void cursorKeyPress(eFuncKeyCode c);

public slots:
    virtual void onActivePage(int id); //激活当前功能页面的槽
    virtual void onExecuteFunc(int id){Q_UNUSED(id);};//执行功能条件触发的槽  自定义 传入id号用于多个页面共享槽时候处理
    virtual void onExecuteFunc(bool ck){Q_UNUSED(ck);};//执行功能条件触发的槽 可与click绑定
    virtual void onEnterButtonClick(int id){Q_UNUSED(id);};//进入该功能页面的按钮被按下
    virtual void onEnterButtonClick(bool ck){Q_UNUSED(ck);};//进入该功能页面的按钮被按下

public:
    int id() const;
    QString title() const;
    QString stateStr() const;

protected:
    bool isActive();

protected slots:
    void setId(const int newValue);
    void setTitle(const QString newValue);

protected slots:
    void setStateStr(const QString str);
    void setActive(bool act = false);

protected slots:
    virtual void onNewInput(QChar c){Q_UNUSED(c)};
    virtual void onFuncKeyPress(eFuncKeyCode c){Q_UNUSED(c)};

private slots:


protected:
    int m_id; //页面的id号
    QString m_title;//页面的标题
    QString m_stateStr;//当前页面的状态描述字符
    bool m_isActive;//当前是否为激活的页面
};

#endif // CCHILDQWIDGET_H

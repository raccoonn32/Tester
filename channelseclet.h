#ifndef CHANNELSECLET_H
#define CHANNELSECLET_H

#include <QWidget>
#include <QGridLayout>
#include <QKeyEvent>

#include "channelindicatorbutton.h"
#include "cchildqwidget.h"

#include "common/confirmw.h"

namespace Ui {
class ChannelSeclet;
}

class ChannelSeclet : public cChildQWidget
{
    Q_OBJECT

public:
    explicit ChannelSeclet(const QString title, int id, QWidget *parent = NULL);
    ~ChannelSeclet();

    void executeFunc();
public slots:
    void currentDisplay(int id, QString vl1, QString vl2);
    void setChannelsState(QVector<ChannelIndicatorButton::estate> sts);
    void setChannelsState(int id,ChannelIndicatorButton::estate st);

    void onActivePage(int id);

    void reflashStates(int type);
private slots:
    void onNewInput(QChar c);
    void onFuncKeyPress(eFuncKeyCode c);
    void onCursorKeyPress(eFuncKeyCode c);
    void onChannelBtnClicked(bool);

    void on_allSelect_clicked();
    void on_noSelect_clicked();
    void on_selswitch_clicked();

private:
    void setCurrentChannelId(int id);
    void updateKeyBoardInput();
    void sumupSelected();
    void setLabels(int id, QString vl1, QString vl2);

private:
    Ui::ChannelSeclet *ui;
    QGridLayout *pLayout;
    QList<ChannelIndicatorButton *> m_channelButtonlist;
    int m_currentFocusButtonId;
    QString m_keyboardinput;
    int m_keyboardpresetid;//键盘预设的id
    int m_selectedTotal;//已经选中的总数
    int m_channelType;
};

#endif // CHANNELSECLET_H

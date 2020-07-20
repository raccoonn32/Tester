#ifndef BRIDGECIRCUIT_H
#define BRIDGECIRCUIT_H

#include <QTableWidgetItem>
#include <QScrollBar>
#include "basetable.h"


class BridgeCircuit : public BaseTableWidget
{
    Q_OBJECT

public:
    explicit BridgeCircuit(const QString title, int id, QWidget *parent = NULL);
    ~BridgeCircuit();

    void executeFunc();
public slots:
    void onCurrentChannelChanged(int id, QString vl);
    void onCurrentChannelChanged(int channel, bool b, float dat);
    void onCurrentChannelChanged(int id);
    void onActivePage(int id);
    void onTestOver();
    void onCurrentTest(int i);
};

#endif // BRIDGECIRCUIT_H

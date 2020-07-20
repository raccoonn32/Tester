#ifndef INSULATION_H
#define INSULATION_H

#include <QWidget>
#include "basetable.h"

#include "globalcommon.h"

namespace Ui {
class Insulation;
}

class Insulation : public BaseTableWidget
{
    Q_OBJECT

public:
    explicit Insulation(const QString title, int id, QWidget *parent = NULL);
    ~Insulation();

    void executeFunc();
public slots:
    void onInsulationVoltageChanged(quint8 vol);
    void onCurrentChannelChanged(int id, QString vl);
    void onCurrentChannelChanged(int id);
    void onCurrentChannelChanged(int id, bool b ,float dat);
    void onActivePage(int id);
    void onTestOver();
    void onCurrentTest(int i);
protected:

};

#endif // INSULATION_H

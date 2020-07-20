#ifndef VOLTAGESET_H
#define VOLTAGESET_H

#include <QWidget>

#include "cchildqwidget.h"
#include "globalcommon.h"

namespace Ui {
class VoltageSet;
}

class VoltageSet : public cChildQWidget
{
    Q_OBJECT

public:
    explicit VoltageSet(const QString title, int id, QWidget *parent = NULL);
    ~VoltageSet();

    void executeFunc();

   static unsigned char bcd2decimal(unsigned char bcd);
   static unsigned char decimal2bcd(unsigned char decimal);
public slots:
    void setPreVoltage();

    void onEnterButtonClick(bool ck);
private slots:
    void onNewInput(QChar c);
    void onFuncKeyPress(eFuncKeyCode c);

private:
    void updateKeyBoardInput();
    void setPreVoltage(int v);
private:
    Ui::VoltageSet *ui;
    QString m_keyboardinput;
    int m_preVoltage;
    QVector<int> voltageList;
};

#endif // VOLTAGESET_H

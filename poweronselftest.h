#ifndef POWERONSELFTEST_H
#define POWERONSELFTEST_H

#include <QWidget>

#include "globalcommon.h"
#include "cchildqwidget.h"

namespace Ui {
class PowerOnSelfTest;
}

class PowerOnSelfTest : public cChildQWidget
{
    Q_OBJECT

public:
    explicit PowerOnSelfTest(const QString title, int id, QWidget *parent = NULL);
    ~PowerOnSelfTest();


private slots:
    void onPwSelfTestResultChanged();

private:
    Ui::PowerOnSelfTest *ui;
};

#endif // POWERONSELFTEST_H

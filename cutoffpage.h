#ifndef CUTOFFPAGE_H
#define CUTOFFPAGE_H

#include <QWidget>
#include "cchildqwidget.h"
#include "globalcommon.h"

namespace Ui {
class CutOffPage;
}

class CutOffPage : public cChildQWidget
{
    Q_OBJECT

public:
    enum eCutOffSta { Unexecuted = 1,Disconnected};
    explicit CutOffPage(QWidget *parent = NULL);
    explicit CutOffPage(const QString title, int id, QWidget *parent = NULL);
    ~CutOffPage();

    void executeFunc();
public slots:
    void onCutOffOver();
private:
    void setCutOffState(eCutOffSta st = Unexecuted);

private:
    Ui::CutOffPage *ui;
    eCutOffSta m_sta;
};

#endif // CUTOFFPAGE_H

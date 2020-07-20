#ifndef RELEASEPAGE_H
#define RELEASEPAGE_H

#include <QWidget>
#include "basetable.h"

#include "common/confirmw.h"

namespace Ui {
class ReleasePage;
}

class ReleasePage : public BaseTableWidget
{
    Q_OBJECT

public:
    explicit ReleasePage(const QString title, int id,QWidget *parent = NULL);
    ~ReleasePage();

    void executeFunc();
public slots:
    void onCurrentChannelChanged(int id, QString vl);
    void onCurrentChannelChanged(int id);
    void onActivePage(int id);
    void onCurrentChannelChanged(int id, bool b, float dat);
    void onTestOver();
    void onCurrentTest(int i);
};

#endif // RELEASEPAGE_H

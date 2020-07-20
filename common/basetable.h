#ifndef BASETABLEWIDGET_H
#define BASETABLEWIDGET_H

#include <QTableWidgetItem>
#include <QScrollBar>
#include <QWidget>
#include "cchildqwidget.h"

//注意文件名大小写

namespace Ui {
class BaseTableWidget;
}

class BaseTableWidget : public cChildQWidget
{
    Q_OBJECT

public:
    explicit BaseTableWidget(const QString title, int id, QWidget *parent = NULL);
    virtual ~BaseTableWidget();

signals:
    void enterKeyPressed(); //传递到外部
    void escKeyPressed(); //传递到外部

protected:
    void setLeftLabel(QString str);
    void setLeftValue(QString str);
    void setRightLabel(QString str);
    void setRightValue(QString str);
    void setItemText(int id,QString str);
    void resetItemText(QString str);
    void setUnitLabel(QString str);
    QString itemText(int id) const;

    const QTableWidgetItem &item(int id) const;
private:
    void initTable();
    void updateKeyBoardInput();
private slots:
    void onNewInput(QChar c);
    void onFuncKeyPress(eFuncKeyCode c);
    void onCursorKeyPress(eFuncKeyCode c);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::BaseTableWidget *ui;
    //对应每个通道的item
    QVector<QTableWidgetItem*> items;

    QString m_keyboardinput;
};

#endif // BASETABLEWIDGET_H

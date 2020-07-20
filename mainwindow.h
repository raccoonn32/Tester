#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QShortcut>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QFileDialog>
#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include "globalcommon.h"
#include "modbus_backend.h"
#include "poweronselftest.h"
#include "channeltest.h"
#include "voltageset.h"
#include "channelseclet.h"
#include "bridgecircuit.h"
#include "insulation.h"
#include "releasepage.h"
#include "cutoffpage.h"

#include "newthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setAppStatesString(const QString &str);
    void setFuncTitle(const QString &str);

public slots:
    void onTitleStringChanged(const QString &str);
    void onChildStateStringChanged(const QString &str);

private:
    void initBtns();
    void setSelfTestState();
    void saveResultFile(BackWorkThread::eWorkType typede);

    bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);
    bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist);
    bool removeFolderContent(const QString &folderDir);

    QStringList getFilenames(const QString &folderDir);
    void mountExport();

private slots:
    void onFuncBtnClick(int id);

    void on_pb_execute_clicked();

    void onPwSelfTestResultChanged();
    void onSelfTestOver(int type);
    
    void quitAPP();
    void on_pb_TEST_clicked();

    void busy();
    void unBusy();

    void on_pb_export_clicked();

    void connectionSlot();
    void functionSelSlot(int item);
    void functionExecute();
    void functionLedStatus();
    void stopTimerSlot();

private:
    Ui::MainWindow *ui;
    QButtonGroup *m_btnGroup;

//    modbus_backend *mb;

    PowerOnSelfTest *m_selfTest;
    ChannelTest *m_channeltestpage, *m_channeltestpage2;
    VoltageSet *m_voltagesetPage;
    ChannelSeclet *m_channelselect;
    BridgeCircuit *m_bridgecircuit;
    Insulation *m_insulation;
    ReleasePage *m_releasepage;
    CutOffPage *m_cutoffpage;
    QShortcut *quit;
    //QDateTime m_time;

    QTcpServer *m_tcpserver;
    QTcpSocket *m_tcpsocket;

    ThreadFromQThread *m_newthread;
    QTimer *m_timer;
};

#endif // MAINWINDOW_H

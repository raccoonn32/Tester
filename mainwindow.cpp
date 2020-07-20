#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qapphelper.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initBtns(); //初始化功能按钮
    quit = new QShortcut(this);
    quit->setKey(Qt::CTRL + Qt::Key_Q);
    quit->setAutoRepeat(false);
    connect(quit,SIGNAL(activated()),this,SLOT(quitAPP()));    //快捷键退出程序

    ui->pb_TEST->setHidden(true);
#ifdef TEST_
    ui->pb_TEST->setHidden(false);
#endif

    //绑定信号
    connect(&GlobalCommon::GetInstance(),SIGNAL(running()),this,SLOT(busy()));
    connect(&GlobalCommon::GetInstance(),SIGNAL(runover()),this,SLOT(unBusy()));
    //页面配置
    if(1){
        m_selfTest = new PowerOnSelfTest(tr("开机自检"),SELFTEST_PAGE,ui->stackedWidget);
        ui->stackedWidget->insertWidget(SELFTEST_PAGE,m_selfTest);
        //由于stackedWidget 切换机制的特殊原因不能触发change事件 此处需手动调用
        connect(m_selfTest,SIGNAL(appTitleString(QString)),this,SLOT(onTitleStringChanged(QString)));
        connect(m_selfTest,SIGNAL(appStatesString(QString)),this,SLOT(onChildStateStringChanged(QString)));
        onTitleStringChanged(m_selfTest->title());
        onChildStateStringChanged(m_selfTest->title());

        m_channeltestpage = new ChannelTest(tr("桥路通道自检"),CHTEST_PAGE,ui->stackedWidget);
        ui->stackedWidget->insertWidget(CHTEST_PAGE,m_channeltestpage);
        connect(m_channeltestpage,SIGNAL(appTitleString(QString)),this,SLOT(onTitleStringChanged(QString)));
        connect(m_channeltestpage,SIGNAL(appStatesString(QString)),this,SLOT(onChildStateStringChanged(QString)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(currentChannelSelfTest(int)),
                m_channeltestpage,SLOT(currentTest(int)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(channelSeflTestRes(int,int,bool,float)),
                m_channeltestpage,SLOT(onchannelSeflTestRes(int,int,bool,float)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(selfTestOver(int)),
                m_channeltestpage,SLOT(onSelfTestOver(int)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(selfTestOver(int)),
                this,SLOT(onSelfTestOver(int)));

        //保存桥路通道自检线阻值
        connect(&GlobalCommon::GetInstance(),SIGNAL(selfTestOver(int)),
                m_channeltestpage,SLOT(settingTestValue(int)));

        //桥路和绝缘自检单独检测
        m_channeltestpage2 = new ChannelTest(tr("绝缘通道自检"),CHTEST_PAGE2,ui->stackedWidget);
        ui->stackedWidget->insertWidget(CHTEST_PAGE2,m_channeltestpage2);
        connect(m_channeltestpage2,SIGNAL(appTitleString(QString)),this,SLOT(onTitleStringChanged(QString)));
        connect(m_channeltestpage2,SIGNAL(appStatesString(QString)),this,SLOT(onChildStateStringChanged(QString)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(currentChannelSelfTest(int)),
                m_channeltestpage2,SLOT(currentTest(int)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(channelSeflTestRes(int,int,bool,float)),
                m_channeltestpage2,SLOT(onchannelSeflTestRes(int,int,bool,float)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(selfTestOver(int)),
                m_channeltestpage2,SLOT(onSelfTestOver(int)));
//        connect(&GlobalCommon::GetInstance(),SIGNAL(selfTestOver(int)),
//                this,SLOT(onSelfTestOver(int)));


        //保存绝缘通道自检线阻值
        connect(&GlobalCommon::GetInstance(),SIGNAL(selfTestOver(int)),
                m_channeltestpage2,SLOT(settingTestValue(int)));

        m_voltagesetPage = new VoltageSet(tr("绝缘电压设置"),VOLSET_PAGE,ui->stackedWidget);
        ui->stackedWidget->insertWidget(VOLSET_PAGE,m_voltagesetPage);
        connect(m_voltagesetPage,SIGNAL(appTitleString(QString)),this,SLOT(onTitleStringChanged(QString)));
        connect(m_voltagesetPage,SIGNAL(appStatesString(QString)),this,SLOT(onChildStateStringChanged(QString)));
        //根据功能 绑定  点击一次对应按钮响应一次
        connect(ui->pb_voltageSet,SIGNAL(clicked(bool)),m_voltagesetPage,SLOT(onEnterButtonClick(bool)));

        m_channelselect = new ChannelSeclet(tr("通道选择"),CHSEL_PAGE,ui->stackedWidget);
        ui->stackedWidget->insertWidget(CHSEL_PAGE,m_channelselect);
        connect(m_channelselect,SIGNAL(appTitleString(QString)),this,SLOT(onTitleStringChanged(QString)));
        connect(m_channelselect,SIGNAL(appStatesString(QString)),this,SLOT(onChildStateStringChanged(QString)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(selfTestOver(int)),
                m_channelselect,SLOT(reflashStates(int)));


        m_bridgecircuit = new BridgeCircuit(tr("桥路电阻测量"),BRIDGE_PAGE,ui->stackedWidget);
        ui->stackedWidget->insertWidget(BRIDGE_PAGE,m_bridgecircuit);
        connect(m_bridgecircuit,SIGNAL(appTitleString(QString)),this,SLOT(onTitleStringChanged(QString)));
        connect(m_bridgecircuit,SIGNAL(appStatesString(QString)),this,SLOT(onChildStateStringChanged(QString)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(bridgeTestOver()),
                m_bridgecircuit,SLOT(onTestOver()));
        connect(&GlobalCommon::GetInstance(),SIGNAL(channelBridgeTestRes(int,bool,float)),
                m_bridgecircuit,SLOT(onCurrentChannelChanged(int,bool,float)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(currentBridgeTest(int)),
                m_bridgecircuit,SLOT(onCurrentTest(int)));

        m_insulation = new Insulation(tr("绝缘电阻测量"),INSULATION_PAGE,ui->stackedWidget);
        ui->stackedWidget->insertWidget(INSULATION_PAGE,m_insulation);
        connect(m_insulation,SIGNAL(appTitleString(QString)),this,SLOT(onTitleStringChanged(QString)));
        connect(m_insulation,SIGNAL(appStatesString(QString)),this,SLOT(onChildStateStringChanged(QString)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(channelInsTestRes(int,bool,float)),
                m_insulation,SLOT(onCurrentChannelChanged(int,bool,float)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(insulationTestOver()),
                 m_insulation,SLOT(onTestOver()));
        connect(&GlobalCommon::GetInstance(),SIGNAL(insulationVoltageChanged(quint8)),
                m_insulation,SLOT(onInsulationVoltageChanged(quint8)));
        connect(&GlobalCommon::GetInstance(),SIGNAL(currentInsTest(int)),
                m_insulation,SLOT(onCurrentTest(int)));

//        m_releasepage = new ReleasePage(tr("泄放"),RELEASE_PAGE,ui->stackedWidget);
//        ui->stackedWidget->insertWidget(RELEASE_PAGE,m_releasepage);
//        connect(m_releasepage,SIGNAL(appTitleString(QString)),this,SLOT(onTitleStringChanged(QString)));
//        connect(m_releasepage,SIGNAL(appStatesString(QString)),this,SLOT(onChildStateStringChanged(QString)));
//        connect(&GlobalCommon::GetInstance(),SIGNAL(channelReleaseTestRes(int,bool,float)),
//                m_releasepage,SLOT(onCurrentChannelChanged(int,bool,float)));
//        connect(&GlobalCommon::GetInstance(),SIGNAL(releaseOver()),
//                m_releasepage,SLOT(onTestOver()));
//        connect(&GlobalCommon::GetInstance(),SIGNAL(currentReleaseTest(int)),
//                m_releasepage,SLOT(onCurrentTest(int)));

        m_cutoffpage = new CutOffPage(tr("切断通道"),CUTOFF_PAGE,ui->stackedWidget);
        ui->stackedWidget->insertWidget(CUTOFF_PAGE,m_cutoffpage);
        connect(&GlobalCommon::GetInstance(),SIGNAL(cutoffOver()),
                m_cutoffpage,SLOT(onCutOffOver()));

        //绑定 主动修改
        connect(m_cutoffpage,SIGNAL(appTitleString(QString)),this,SLOT(onTitleStringChanged(QString)));
        connect(m_cutoffpage,SIGNAL(appStatesString(QString)),this,SLOT(onChildStateStringChanged(QString)));
    }

    if(1) //自检UI更新
    {
        connect(&GlobalCommon::GetInstance(),SIGNAL(pwSelfTestResultChanged()),this,SLOT(onPwSelfTestResultChanged()));
        connect(&GlobalCommon::GetInstance(),SIGNAL(pwSelfTestResultChanged()),m_selfTest,SLOT(onPwSelfTestResultChanged()));
    }

    if(1) //桥路测量更新
    {
        connect(&GlobalCommon::GetInstance(),SIGNAL(britestProgessChanged(int)),m_bridgecircuit,SLOT(onCurrentChannelChanged(int)));
    }

    //tcpserver
    m_tcpsocket = NULL;
    m_tcpserver = new QTcpServer(this);
    //监听套接字
    m_tcpserver->listen(QHostAddress::Any, PORT);
    //连接
    connect(m_tcpserver, SIGNAL(newConnection()), this, SLOT(connectionSlot()));


#if !defined(Q_OS_WIN)
    //gpio telnet
    m_newthread = new ThreadFromQThread(this);
    m_newthread->start();
    connect(m_newthread, SIGNAL(functionSelect(int)), this, SLOT(functionSelSlot(int)));
    connect(m_newthread, SIGNAL(telnetexecute()), this, SLOT(functionExecute()));

    //select function indicate led off
    gpio gpioled;
    gpioled.setGpio(LED1, true, true);
    gpioled.setGpio(LED2, true, true);
    gpioled.setGpio(LED3, true, true);
    gpioled.setGpio(LED4, true, true);
#endif

    //telnet execute led on-off-on
    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(functionLedStatus()));
    connect(&GlobalCommon::GetInstance(), SIGNAL(selfTestOver(int)), this, SLOT(stopTimerSlot()));
    connect(&GlobalCommon::GetInstance(), SIGNAL(bridgeTestOver()), this, SLOT(stopTimerSlot()));
    connect(&GlobalCommon::GetInstance(), SIGNAL(insulationTestOver()), this, SLOT(stopTimerSlot()));

#ifndef TEST_
    //程序运行后自动开始自检
    GlobalCommon::GetInstance().startWork(BackWorkThread::PWONSELFTEST);
#else
    ui->widgetBtns->setEnabled(true); //调试测试用
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

//功能按键按下 事件更新
void MainWindow::onFuncBtnClick(int id)
{
    int currentIndex = ui->stackedWidget->currentIndex();
    if (currentIndex == id) return;
    ui->stackedWidget->setCurrentIndex(id);

    if(id == CHTEST_PAGE)   //按下通道自检即执行命令
    {
        m_channeltestpage->preExecute();
    }
    else if(id == CHTEST_PAGE2)
    {
        m_channeltestpage2->preExecute();
    }

}

//btns初始化设置
void MainWindow::initBtns()
{
    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->setExclusive(true); //设置为互斥
    m_btnGroup->addButton(ui->pb_channelTest,1);
    m_btnGroup->addButton(ui->pb_voltageSet,2);
    m_btnGroup->addButton(ui->pb_channelSelect,3);
    m_btnGroup->addButton(ui->pb_bridgeCircuit,4);
    m_btnGroup->addButton(ui->pb_insulation,5);
    //m_btnGroup->addButton(ui->pb_release,6);
    m_btnGroup->addButton(ui->pb_cutOff,6);
    m_btnGroup->addButton(ui->pb_channelTestIns, 7);

    ui->widgetBtns->setFocusPolicy(Qt::NoFocus);
    ui->pb_channelTest->setFocusPolicy(Qt::NoFocus);
    ui->pb_voltageSet->setFocusPolicy(Qt::NoFocus);
    ui->pb_bridgeCircuit->setFocusPolicy(Qt::NoFocus);
    ui->pb_channelSelect->setFocusPolicy(Qt::NoFocus);
    ui->pb_bridgeCircuit->setFocusPolicy(Qt::NoFocus);
    ui->pb_insulation->setFocusPolicy(Qt::NoFocus);
    //ui->pb_release->setFocusPolicy(Qt::NoFocus);
    ui->pb_cutOff->setFocusPolicy(Qt::NoFocus);
    ui->pb_execute->setFocusPolicy(Qt::NoFocus);

    ui->pb_channelTestIns->setFocusPolicy(Qt::NoFocus);


    ui->pb_channelTest->setCheckable(true);
    ui->pb_voltageSet->setCheckable(true);
    ui->pb_channelSelect->setCheckable(true);
    ui->pb_bridgeCircuit->setCheckable(true);
    ui->pb_insulation->setCheckable(true);
    //ui->pb_release->setCheckable(true);
    ui->pb_cutOff->setCheckable(true);
    ui->pb_channelTestIns->setCheckable(true);

    ui->pb_channelTest->setChecked(false);
    ui->pb_voltageSet->setChecked(false);
    ui->pb_channelSelect->setChecked(false);
    ui->pb_bridgeCircuit->setChecked(false);
    ui->pb_insulation->setChecked(false);
    //ui->pb_release->setChecked(false);
    ui->pb_cutOff->setChecked(false);
    ui->pb_channelTestIns->setChecked(false);
    connect(m_btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(onFuncBtnClick(int)));

    //ui->widgetBtns->setEnabled(false);//自检通过 才能使能该按键组

    ui->pb_channelTest->setShortcut(Qt::Key_F1);
    ui->pb_voltageSet->setShortcut(Qt::Key_F2);
    ui->pb_channelSelect->setShortcut(Qt::Key_F3);
    ui->pb_bridgeCircuit->setShortcut(Qt::Key_F4);
    ui->pb_insulation->setShortcut(Qt::Key_F5);
    //ui->pb_release->setShortcut(Qt::Key_F6);
    ui->pb_cutOff->setShortcut(Qt::Key_F7);
    ui->pb_channelTestIns->setShortcut(Qt::Key_F8);
    ui->pb_execute->setShortcut(Qt::Key_F9);

//    //状态栏
//    {
//        QSettings settings("./config.conf",QSettings::IniFormat);
//        settings.beginGroup("Config");
//        QString restesterconfig = settings.value("restester").toString();
//        QString docontrollerconfig =  settings.value("docontroller").toString();
//        settings.endGroup();
//        ui->lbCtrlDev->setText(ui->lbCtrlDev->text()+ docontrollerconfig);
//        ui->lbTesterDev->setText(ui->lbTesterDev->text()+ restesterconfig);
//    }
}



void MainWindow::setFuncTitle(const QString &str)
{
    ui->lb_funTitle->setText(str);
}

void MainWindow::onTitleStringChanged(const QString &str)
{
    setFuncTitle(str);
}



//用于处理子页面的 状态发生变化
void MainWindow::onChildStateStringChanged(const QString &str)
{
    setAppStatesString(str);
}


void MainWindow::setAppStatesString(const QString &str)
{
    ui->lb_appstates->setText(str);
}

//执行按键按下将会根据状态执行不同的功能
void MainWindow::on_pb_execute_clicked()
{
    if(ui->stackedWidget->currentIndex() == SELFTEST_PAGE)
        m_selfTest->executeFunc();
    else if(ui->stackedWidget->currentIndex() == CHTEST_PAGE)
        m_channeltestpage->executeFunc(0);
    else if(ui->stackedWidget->currentIndex() == VOLSET_PAGE)
        m_voltagesetPage->executeFunc();
    else if(ui->stackedWidget->currentIndex() == CHSEL_PAGE)
        m_channelselect->executeFunc();
    else if(ui->stackedWidget->currentIndex() == BRIDGE_PAGE)
        m_bridgecircuit->executeFunc();
    else if(ui->stackedWidget->currentIndex() == INSULATION_PAGE)
        m_insulation->executeFunc();
//    else if(ui->stackedWidget->currentIndex() == RELEASE_PAGE)
//        m_releasepage->executeFunc();
    else if(ui->stackedWidget->currentIndex() == CUTOFF_PAGE)
        m_cutoffpage->executeFunc();
    else if(ui->stackedWidget->currentIndex() == CHTEST_PAGE2)
        m_channeltestpage2->executeFunc(1);
}


void MainWindow::onPwSelfTestResultChanged()
{
    GlobalCommon::SelfTestResult cres = GlobalCommon::GetInstance().controllerSelfTestResult();
    GlobalCommon::SelfTestResult dres = GlobalCommon::GetInstance().detectorSelfTestResult();

    if(dres ==  GlobalCommon::detectorOk &&  cres ==  GlobalCommon::controllerOk )
    {
        //ui->widgetBtns->setEnabled(true);
        ui->pb_channelTest->setEnabled(true);
        ui->pb_voltageSet->setEnabled(true);
        ui->pb_bridgeCircuit->setEnabled(false);
        ui->pb_channelSelect->setEnabled(false);
        ui->pb_bridgeCircuit->setEnabled(false);
        //ui->pb_release->setEnabled(false);
        ui->pb_insulation->setEnabled(false);
        ui->pb_cutOff->setEnabled(false);
        ui->pb_execute->setEnabled(true);
        ui->pb_channelTestIns->setEnabled(true);

        //m_newthread->start();

        //GlobalCommon::GetInstance().getSelfTestRValue();
        //onSelfTestOver(1);

    }else
    {
        //ui->widgetBtns->setEnabled(false);
        ui->pb_channelTest->setEnabled(false);
        ui->pb_voltageSet->setEnabled(false);
        ui->pb_bridgeCircuit->setEnabled(false);
        ui->pb_channelSelect->setEnabled(false);
        ui->pb_bridgeCircuit->setEnabled(false);
        //ui->pb_release->setEnabled(false);
        ui->pb_insulation->setEnabled(false);
        ui->pb_cutOff->setEnabled(false);
        ui->pb_execute->setEnabled(false);
        ui->pb_channelTestIns->setEnabled(false);
        ui->pb_export->setEnabled(true);
    }
}


void MainWindow::onSelfTestOver(int)
{

    ui->widgetBtns->setEnabled(true);
    ui->pb_channelTest->setEnabled(true);
    ui->pb_voltageSet->setEnabled(true);
    ui->pb_bridgeCircuit->setEnabled(true);
    ui->pb_channelSelect->setEnabled(true);
    ui->pb_bridgeCircuit->setEnabled(true);
    ui->pb_insulation->setEnabled(true);
    //ui->pb_release->setEnabled(true);
    ui->pb_cutOff->setEnabled(true);
    ui->pb_execute->setEnabled(true);
    ui->pb_channelTestIns->setEnabled(true);


}

void MainWindow::quitAPP()
{
    qApp->quit();
}

void MainWindow::on_pb_TEST_clicked()
{
    qDebug() << __PRETTY_FUNCTION__ ;
    GlobalCommon::GetInstance().startWork(BackWorkThread::FUNCTEST);
}

void MainWindow::busy()
{
    GlobalCommon::SelfTestResult cres = GlobalCommon::GetInstance().controllerSelfTestResult();
    GlobalCommon::SelfTestResult dres = GlobalCommon::GetInstance().detectorSelfTestResult();
    if(dres !=  GlobalCommon::detectorOk | cres !=  GlobalCommon::controllerOk ) return;
    ui->widgetBtns->setEnabled(false);
    qDebug() << __FUNCTION__;
}

void MainWindow::unBusy()
{
    qDebug() << __PRETTY_FUNCTION__;
    GlobalCommon::SelfTestResult cres = GlobalCommon::GetInstance().controllerSelfTestResult();
    GlobalCommon::SelfTestResult dres = GlobalCommon::GetInstance().detectorSelfTestResult();
    if(dres !=  GlobalCommon::detectorOk | cres !=  GlobalCommon::controllerOk ) return;
    ui->widgetBtns->setEnabled(true);
}

void MainWindow::mountExport()
{
    //    QString destDirPath = QFileDialog::getExistingDirectory(
    //                this, "选择导出的文件夹",
    //                 "/");

    QString destDirPath = "/mnt";
    system("mount /dev/sda4  /mnt");


    qDebug() << __FUNCTION__ ;

    //QDir dir = GlobalCommon::GetInstance().getRecordDir();
    QString srcdir = QDir::currentPath() + "/record";
    bool res = copyDirectoryFiles(srcdir,destDirPath,true);

    if(res){
        system("umount /mnt");
        QAppHelper::ShowMessageBoxInfo("导出完成!");
    }
    else
        QAppHelper::ShowMessageBoxError("导出错误!");

    if(res)
    {
        int choose = QAppHelper::ShowMessageBoxQuesion("需要删除仪器存储的记录文件吗?");

        if(choose == QMessageBox::AcceptRole)
        {
            //删除本地文件夹
            removeFolderContent(srcdir);
        }
    }
}

void MainWindow::on_pb_export_clicked()
{
    //mountExport();

    QString srcdir = QDir::currentPath() + "/record";
    QStringList fileList;

    fileList = getFilenames(srcdir);

    if(m_tcpsocket == NULL)
    {
         QAppHelper::ShowMessageBoxInfo("以太网未连接！！！");
         return;
    }

    for(int i = 0; i < fileList.size(); i++){
        qDebug() << fileList.at(i);
        QString dirname = fileList.at(i);
        QString tmp = dirname + "\n";
        m_tcpsocket->write(tmp.toUtf8().data());

        QFile file(dirname);
        if(file.open(QFile::ReadOnly)){
            QByteArray data = file.readAll();
            m_tcpsocket->write(data);
            file.close();
        }
    }

    int choose = QAppHelper::ShowMessageBoxQuesion("文件传输完成，需要删除仪器存储的记录文件吗?");

    if(choose == QMessageBox::AcceptRole)
    {
        //删除本地文件夹
        removeFolderContent(srcdir);
    }

}

//拷贝文件：
bool MainWindow::copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir)){
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist){
        if(coverFileIfExist){
            createfile->remove(toDir);
        }
    }//end if

    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}

//拷贝文件夹：
bool MainWindow::copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if(!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
        if(!targetDir.mkdir(targetDir.absolutePath()))
            return false;
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir()){    /**< 当为目录时，递归的进行copy */
            if(!copyDirectoryFiles(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()),
                coverFileIfExist))
                return false;
        }
        else{            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
            if(coverFileIfExist && targetDir.exists(fileInfo.fileName())){
                targetDir.remove(fileInfo.fileName());
            }

            /// 进行文件copy
            if(!QFile::copy(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()))){
                    return false;
            }
        }
    }
    return true;
}

// ≈ rm -f
bool MainWindow::removeFolderContent(const QString &folderDir)
{
    QDir dir(folderDir);
    QFileInfoList fileList;
    QFileInfo curFile;
    if(!dir.exists())  {return false;}//文件不存，则返回false
    fileList=dir.entryInfoList(QDir::Dirs|QDir::Files
                               |QDir::Readable|QDir::Writable
                               |QDir::Hidden|QDir::NoDotAndDotDot
                               ,QDir::Name);
    while(fileList.size()>0)
    {
        int infoNum=fileList.size();
        for(int i=infoNum-1;i>=0;i--)
        {
            curFile=fileList[i];
            if(curFile.isFile())//如果是文件，删除文件
            {
                QFile fileTemp(curFile.filePath());
                fileTemp.remove();
                fileList.removeAt(i);
            }
            if(curFile.isDir())//如果是文件夹
            {
                QDir dirTemp(curFile.filePath());
                QFileInfoList fileList1=dirTemp.entryInfoList(QDir::Dirs|QDir::Files
                                                              |QDir::Readable|QDir::Writable
                                                              |QDir::Hidden|QDir::NoDotAndDotDot
                                                              ,QDir::Name);
                if(fileList1.size()==0)//下层没有文件或文件夹
                {
                    dirTemp.rmdir(".");
                    fileList.removeAt(i);
                }
                else//下层有文件夹或文件
                {
                    for(int j=0;j<fileList1.size();j++)
                    {
                        if(!(fileList.contains(fileList1[j])))
                            fileList.append(fileList1[j]);
                    }
                }
            }
        }
    }
    return true;
}

void MainWindow::connectionSlot()
{
    //取出建立好的套接字
    m_tcpsocket = m_tcpserver->nextPendingConnection();

    QString ip = m_tcpsocket->peerAddress().toString();
    qint16 port = m_tcpsocket->peerPort();
    QString coninfo = QString("[%1:%2]:成功连接！").arg(ip).arg(port);

    QAppHelper::ShowMessageBoxInfo(coninfo);
}

QStringList MainWindow::getFilenames(const QString &folderDir)
{
    QDir dir(folderDir);
    QFileInfoList fileInfoList, filenamelist;
    QFileInfo fileInfo;

    static QStringList filePathList;

    fileInfoList = dir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir()){
            getFilenames(fileInfo.filePath());
        }
        else{
            filenamelist.append(fileInfo);
        }

    }

    for(int i = 0; i < filenamelist.size(); i++){
        fileInfo = filenamelist[i];
        filePathList.append(fileInfo.filePath());
    }

    return filePathList;
}

void MainWindow::functionSelSlot(int item)
{
    qDebug() << "item:" << item;

    gpio gpio_led;

    if(item == 1)
    {
        ui->stackedWidget->setCurrentIndex(1);
        gpio_led.setGpio(LED1, false, true);
        gpio_led.setGpio(LED2, true, true);
        gpio_led.setGpio(LED3, true, true);
        gpio_led.setGpio(LED4, true, true);
    }
    else if(item == 2)
    {
        ui->stackedWidget->setCurrentIndex(7);
        gpio_led.setGpio(LED1, true, true);
        gpio_led.setGpio(LED2, false, true);
        gpio_led.setGpio(LED3, true, true);
        gpio_led.setGpio(LED4, true, true);
    }
    else if(item == 3)
    {
        ui->stackedWidget->setCurrentIndex(4);        
        gpio_led.setGpio(LED1, true, true);
        gpio_led.setGpio(LED2, true, true);
        gpio_led.setGpio(LED3, false, true);
        gpio_led.setGpio(LED4, true, true);
    }
    else if(item == 4)
    {
        ui->stackedWidget->setCurrentIndex(5);        
        gpio_led.setGpio(LED1, true, true);
        gpio_led.setGpio(LED2, true, true);
        gpio_led.setGpio(LED3, true, true);
        gpio_led.setGpio(LED4, false, true);
    }
    else if(item == 5)
    {
        ui->stackedWidget->setCurrentIndex(5);
        gpio_led.setGpio(LED1, true, true);
        gpio_led.setGpio(LED2, true, true);
        gpio_led.setGpio(LED3, true, true);
        gpio_led.setGpio(LED4, true, true);
    }

}

void MainWindow::functionExecute()
{
    //start led on-off-on
    m_timer->start();

    for(int i = 0; i < TOTALCHANNEL; i++)
        GlobalCommon::GetInstance().setChannelSelected(i);

    if(ui->stackedWidget->currentIndex() == CHTEST_PAGE)
        GlobalCommon::GetInstance().startWork(BackWorkThread::CHSELFTEST);
    if(ui->stackedWidget->currentIndex() == CHTEST_PAGE2)
        GlobalCommon::GetInstance().startWork(BackWorkThread::CHSELFTESTINS);
    if(ui->stackedWidget->currentIndex() == BRIDGE_PAGE)
        GlobalCommon::GetInstance().startWork(BackWorkThread::BRITEST);
    if(ui->stackedWidget->currentIndex() == INSULATION_PAGE)
        GlobalCommon::GetInstance().startWork(BackWorkThread::INSTEST);

}

void MainWindow::functionLedStatus()
{
#ifdef IMX_GPIO
    gpio gpio_led;

    static bool bledsta = false;

    bledsta = !bledsta;

    if(ui->stackedWidget->currentIndex() == CHTEST_PAGE)
    {
        gpio_led.setGpio(LED1, bledsta);
    }
    if(ui->stackedWidget->currentIndex() == CHTEST_PAGE2)
    {
        gpio_led.setGpio(LED2, bledsta);
    }
    if(ui->stackedWidget->currentIndex() == BRIDGE_PAGE)
    {
        gpio_led.setGpio(LED3, bledsta);
    }
    if(ui->stackedWidget->currentIndex() == INSULATION_PAGE)
    {
        gpio_led.setGpio(LED4, bledsta);
    }

#endif
}
void MainWindow::stopTimerSlot()
{
    m_timer->stop();

#ifdef IMX_GPIO
    gpio gpio_led;

    if(ui->stackedWidget->currentIndex() == CHTEST_PAGE)
    {
        gpio_led.setGpio(LED1, false);
    }
    if(ui->stackedWidget->currentIndex() == CHTEST_PAGE2)
    {
        gpio_led.setGpio(LED2, false);
    }
    if(ui->stackedWidget->currentIndex() == BRIDGE_PAGE)
    {
        gpio_led.setGpio(LED3, false);
    }
    if(ui->stackedWidget->currentIndex() == INSULATION_PAGE)
    {
        gpio_led.setGpio(LED4, false);
    }
#endif
}

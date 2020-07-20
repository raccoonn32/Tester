#include "mainwindow.h"
#include <QApplication>

#include "globalcommon.h"
#include "qapphelper.h"

#include "qled.h"

#include "common/confirmw.h"
#include <QtCore/QMetaEnum>
#include <QDebug>

#include "debuglogform.h"

#include <QShortcut>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setvbuf(stderr,NULL,_IONBF,64); //禁止 stderr 缓冲
    setvbuf(stdout,NULL,_IONBF,64); //禁止 stdout 缓冲
#ifdef TEST_
    if(0)//日志文件
#endif
    {
        FILE *streame = freopen("log.log", "w", stderr );   //将出错流和输出流写入日志文件
        FILE *streamo = freopen("log.log", "w", stdout );
        QAppHelper::SetDebugLogFile(true);  //程序调试输出提示
    }

    qRegisterMetaType<DoController::eChannelType>("DoController::eChannelType");//注册eChannelType类型
    qRegisterMetaType<BackWorkThread::eWorkType>("BackWorkThread::eWorkType");//注册 eWorkType 类型
    qRegisterMetaType<GlobalCommon::SelfTestResult>("GlobalCommon::SelfTestResult");//注册 SelfTestResult 类型

    QAppHelper::SetUTF8Code();
    GlobalCommon::GetInstance();

    qDebug()<< " hello QApp build DateTime @" << __DATE__ << __TIME__;

    QString fontstr = QAppHelper::loadFontFamilyFromFiles("./msjh.ttf");
    qDebug()<< "QApp Loading Font " << fontstr;

    if(fontstr.isEmpty())
    {
        QAppHelper::ShowMessageBoxError("font error!");
    }else
    {
        QFont font (fontstr);
        a.setFont(font);
    }
    qDebug()<< "QApp Loaded Font ";

    //加载样式表
    {
        QFile file(":/skin.css");
        if (file.open(QFile::ReadOnly)) {
            QString qss = QLatin1String(file.readAll());
            qApp->setStyleSheet(qss);
            file.close();
        }
    }
    qDebug()<< "QApp Load Skin";
    MainWindow w;
    w.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::Window);

    w.show();
    return a.exec();
}

#ifndef QAPPHELPER_H
#define QAPPHELPER_H

#endif // QAPPHELPER_H


#include <QCoreApplication>
#include <QObject>
#include <QUrl>
#include <QDesktopServices>
#include <QMessageBox>
#include <QMutex>
#include <QFile>
#include <QDateTime>
#include <QProcess>
#include <QSettings>
#include <QFile>
#include <QFont>
#include <QFontDatabase>

#include <QElapsedTimer>
#include <QThread>

#if defined(Q_OS_WIN32)
#include <windows.h>
#endif

#include <QDebug>
#include <stdio.h>
#include <iostream>
using namespace std;

#include <QTextCodec>

class QAppHelper: public QObject
{
public:

#if QT_VERSION < 0x050000
    //设置编码为GB2312
    static void SetGB2312Code()
    {
        QTextCodec *codec=QTextCodec::codecForName("GB2312");
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForCStrings(codec);
        QTextCodec::setCodecForTr(codec);
    }

    //设置编码为UTF8
    static void SetUTF8Code()
    {
        QTextCodec *codec=QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForCStrings(codec);
        QTextCodec::setCodecForTr(codec);
    }
#else
    static void SetGB2312Code() {};
    static void SetUTF8Code() {};
#endif


    //打开网址
    static void OpenUrlOrExe(QString url)
    {
        QDesktopServices::openUrl(QUrl(url));
    }

    //获取当前可执行文件路径
    static QString GetCurrentPath()
    {
        return QString(QCoreApplication::applicationDirPath()+"/");
    }
    //获取当前可执行文件路径含可执行文件
    static QString GetCurrentExePath()
    {
        return QString(QCoreApplication::applicationFilePath());
    }

    //显示信息框，仅确定按钮
    static void ShowMessageBoxInfo(QString info)
    {
        QMessageBox msg;
        msg.setWindowTitle(tr("提示"));
        msg.setText(info);
        msg.setIcon(QMessageBox::Information);
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();
    }

    //显示错误框，仅确定按钮
    static void ShowMessageBoxError(QString info)
    {
        QMessageBox msg;
        msg.setWindowTitle(tr("错误"));
        msg.setText(info);
        msg.setIcon(QMessageBox::Critical);
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();
    }


    //显示询问框，确定和取消按钮
    static int ShowMessageBoxQuesion(QString info)
    {
        QMessageBox msg;
        msg.setWindowTitle(tr("询问"));
        msg.setText(info);
        msg.setIcon(QMessageBox::Question);
        msg.addButton(tr("确定"),QMessageBox::AcceptRole);
        msg.addButton(tr("取消"),QMessageBox::RejectRole);

        return msg.exec();
    }

    /*
     * @breif 将16进制字符串转换为对应的字节序列
     */
   static QByteArray HexStringToByteArray(QString HexString)
    {
        bool ok;
        QByteArray ret;
        HexString = HexString.trimmed();
        HexString = HexString.simplified();
        QStringList sl = HexString.split(" ");

        foreach (QString s, sl) {
            if(!s.isEmpty())
            {
                char c = s.toInt(&ok,16)&0xFF;
                if(ok){
                    ret.append(c);
                }else{
                    qDebug()<<"非法的16进制字符："<<s;
                    QMessageBox::warning(0,tr("错误："),QString("非法的16进制字符: \"%1\"").arg(s));
                }
            }
        }
        qDebug()<<ret;
        return ret;
    }

    //设置调试输出信息产生日志文件
    static void SetDebugLogFile(bool sta=false)
    {
#if QT_VERSION < 0x050000
        if(sta)
            qInstallMsgHandler(QAppHelper::msgOutput);
        else
            qInstallMsgHandler(0);
#else
        if(sta)
            qInstallMessageHandler(QAppHelper::msgOutput);
        else
            qInstallMessageHandler(0);
#endif
    }


    //QString  和 Std string 互相转换
    static QString str2qstr(const string str)
    {
        return QString::fromLocal8Bit(str.data());
    }

    static string qstr2str(const QString qstr)
    {
        QByteArray cdata = qstr.toLocal8Bit();
        return string(cdata);
    }

#if defined(Q_OS_WIN32)
    //获取CpuID信息
    static quint64 GetCpuID(void)
    {
        QString cpu_id = "";
        QProcess p(0);
        p.start("wmic CPU get ProcessorID");
        p.waitForStarted();
        p.waitForFinished();
        cpu_id = QString::fromLocal8Bit(p.readAllStandardOutput());
        cpu_id = cpu_id.remove("ProcessorId").trimmed();
        quint64 cpu_id_val = (quint64)cpu_id.toULongLong(NULL,16);
        return cpu_id_val;
    }
    static QString GetCpuIDString(void)
    {
        QString cpu_id = "";
        QProcess p(0);
        p.start("wmic CPU get ProcessorID");
        p.waitForStarted();
        p.waitForFinished();
        cpu_id = QString::fromLocal8Bit(p.readAllStandardOutput());
        cpu_id = cpu_id.remove("ProcessorId").trimmed();

        return cpu_id;
    }
#else
    static quint64 GetCpuID(void)
    {
        //win32 only
    }
#endif

#if defined(Q_OS_WIN32)
    //设置为只能启动一个程序副本  返回TRUE 程序已经在运行中
    static bool IsAnotherInstanceRunning(void)
    {
        QString str = GetCurrentPath();  //当前程序的启动路径为互斥锁的名
        CreateMutex(NULL, FALSE,str.toStdWString().data() ); //do early
        DWORD  m_dwLastError = GetLastError(); //save for use later...

        return (ERROR_ALREADY_EXISTS == m_dwLastError);
    }

#else
    static bool IsAnotherInstanceRunning(QString strMutexName)
    {
        //win32 only
        return false;
    }
#endif

#if defined(Q_OS_WIN32)
    //写入注册表
    static void writeRegistry(QString str)
    {
        QSettings *reg = new QSettings("HKEY_CURRENT_USER\\Software\\QtApp",QSettings::NativeFormat);
        reg->setValue(str,true);
        delete reg;
    }
    static bool readRegistry(QString str)
    {
        QSettings *reg = new QSettings("HKEY_CURRENT_USER\\Software\\QtApp",QSettings::NativeFormat);
        bool res = reg->value(str,false).toBool();
        delete reg;
        return res;
    }
#else
    static void writeRegistry(QString str)
    {
        //win32 only
    }
#endif

    //设置字体
    static QString loadFontFamilyFromFiles(const QString &fontFilestr)
    {
        QString font = "";

        QFile fontFile(fontFilestr);

        if(!fontFile.open(QIODevice::ReadOnly))
        {
            qDebug()<<"Open font file error";
            return font;
        }

        int loadedFontID = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
        QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
        if(!loadedFontFamilies.empty())
        {
            font = loadedFontFamilies.at(0);
        }
        fontFile.close();
        return font;
    }

    //延时毫秒
    static void delayms(int ms)
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QThread::msleep(ms);
#else
        QElapsedTimer t;
        t.start();
        while(t.elapsed()<ms)
            QCoreApplication::processEvents();
#endif
    }

private:
#if QT_VERSION < 0x050000
    static void msgOutput(QtMsgType type,const char* msg)
#else
    static void msgOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#endif
    {
        // 加锁
        static QMutex mutex;
        mutex.lock();
#if QT_VERSION < 0x050000
        QString localqMsg(msg);
        QByteArray localMsg = localqMsg.toAscii();
#else
        QByteArray localMsg = msg.toLocal8Bit();
#endif
        QString strMsg("");
        switch((int)type)
        {
        case QtDebugMsg:
            strMsg = QString("Debug:");
            break;
        case QtWarningMsg:
            strMsg = QString("Warning:");
            break;
        case QtCriticalMsg:
            strMsg = QString("Critical:");
            break;
        case QtFatalMsg:
            strMsg = QString("Fatal:");
            abort();
            break;
        }

        // 设置输出信息格式
        QString strDateTime = QDateTime::currentDateTime().toString("hh:mm:ss");
#if QT_VERSION < 0x050000
        QString strMessage = QString("%1 %2 DTime:%3")
                .arg(strMsg).arg(localMsg.constData()).arg(strDateTime);
#else
        QString strMessage = QString("%1 %2 File:%3  Line:%4  Func:%5  DTime:%6")
                .arg(strMsg).arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(strDateTime);
#endif

        // 输出信息至文件中（读写、追加形式）
        QFile file(qApp->applicationName()+".log");
        file.open(QIODevice::ReadWrite | QIODevice::Append);
        QTextStream stream(&file);
        stream << strMessage << "\r\n";
        file.flush();
        file.close();
        // 解锁
#if QT_VERSION < 0x050000
        QTextCodec *codec=QTextCodec::codecForName("GB2312");
        QByteArray tp = codec->fromUnicode(strMessage);
        cout << string(tp) << "\n";
#else
        cout << string(localMsg.constData()) << "  @"<< context.file << ":" << context.line << "\n";
#endif
        cout.flush();
        mutex.unlock();
    }
};

//校验组件
class QChecksumHelper: public QObject
{
public:
    static quint16 crc16ForModbus(const QByteArray &data)
    {
        static const quint16 crc16Table_Modbus[] =
        {
            0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
            0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
            0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
            0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
            0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
            0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
            0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
            0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
            0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
            0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
            0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
            0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
            0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
            0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
            0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
            0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
            0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
            0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
            0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
            0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
            0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
            0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
            0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
            0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
            0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
            0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
            0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
            0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
            0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
            0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
            0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
            0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
        };

        quint8 buf;
        quint16 crc16 = 0xFFFF;

        for ( int i = 0; i < data.size(); ++i )
        {
            buf = data.at( i ) ^ crc16;
            crc16 >>= 8;
            crc16 ^= crc16Table_Modbus[ buf ];
        }
        return crc16;
    }

};

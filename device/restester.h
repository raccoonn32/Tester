#ifndef RESTESTER_H
#define RESTESTER_H

#include <QObject>

#include "qmodbusmaster.h"
#include "qmodbusbits.h"

using namespace Modbus;

#define CMD_CHSELFT  (0x01)  //通道自检
#define CMD_TEST     (0x03)  //测试命令
#define CMD_BRI      (0x07)  //桥路命令
#define CMD_INS      (0x0f)  //绝缘命令
#define CMD_CAL      (0x1f)  //校准命令
#define CMD_RES      (0x3f)  //泄放命令

#define VOL_100      (0x10)  //100V
#define VOL_250      (0x25)  //250V
#define VOL_500      (0x50)  //500V


class ResTester : public QObject
{
    Q_OBJECT
public:
    explicit ResTester(const QString &port, int baud = 9600, char pairty = 'N', int databits = 8, int stopbits = 1, QObject *parent = NULL);

signals:

public:
    bool hasError();
public slots:
    bool powerOnSelfTestCMD();
    void setPreChanelTestMode();
    qint8 setTestMode();
    void setWorkMode(int mode);
    void setCurrentChannel(int num);
    void setCurrentVoltage(int vol);
    qint8 runTest(char mod, bool *isCredible, float *res, int voltage=0);

private:
    void sendCurrentMode(char mode);
    bool readTesterResult(QByteArray &dat);
    qint8 readDataAnylasis_(QByteArray a, QByteArray b);
    qint8 readDataStringAnylasis(QString a, QString b);
private:
    QModbusMaster *modbusMaster;
    bool m_hasError;
};

#endif // RESTESTER_H

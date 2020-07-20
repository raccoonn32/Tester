#ifndef MODBUS_BACKEND_H
#define MODBUS_BACKEND_H

#include <QThread>
#include <QString>
#include <QTimer>
#include <QMutex>
#include <QVector>
#include "libmodbus/modbus.h"
class modbus_backend : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(modbus_backend)
public:
    explicit modbus_backend(QObject *parent = 0);
    explicit modbus_backend(QString port,qint16 baud = 9600,
                            char parity = 'N',qint8 databits = 8,
                            qint8 stopbits = 1,bool debugmod = false,QObject *parent = 0);
    virtual ~modbus_backend();

    bool init(QString port, qint16 baud, char parity, qint8 databits, qint8 stopbits, bool debugmod=false);

    void setSlaveAddr(qint8 addr);
    bool isAvailable();
    void setDebugMod(bool mod);
    void setResponseTimeout(long sec=0,long usec=1000000); //默认1000ms
    struct timeval getTimeout();



    //int readBits(int addr, int nb, uint8_t *dest);
    //int readInputBits(int addr, int nb, uint8_t *dest);
    int readRegisters(int addr, int nb);
    //int readInputRegisters(int addr, int nb, uint16_t *dest);
    //int writeBit(int coil_addr, int status);
    int writeRegister(int reg_addr, int value);
    //int writeBits(int addr, int nb, const uint8_t *data);
    //int writeRegisters(int addr, int nb, const uint16_t *data);
    //int maskWriteRegister(int addr, uint16_t and_mask, uint16_t or_mask);
    //int writeAndReadRegisters(int write_addr, int write_nb,
    //                                                    const uint16_t *src, int read_addr, int read_nb,
    //                                                    uint16_t *dest);

signals:
    //该信号用于modbus 回读响应
    //TODO 枚举变量
    void respond(int res);


private:
    void backendInit();
private slots:



private:
    modbus_t * m_mb;
    bool m_mbPortOk;
    QString m_port;
    qint16 m_baud;
    char m_parity;
    qint8 m_stopbits;
    qint8 m_databits;
    qint8 m_slaveaddr;
    struct timeval m_tout; //超时

    QMutex m_mutex;//锁

    QVector<quint16> m_qv_readregs;
};

#endif // MODBUS_BACKEND_H

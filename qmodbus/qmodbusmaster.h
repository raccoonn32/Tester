#ifndef QMODBUSMASTER_H
#define QMODBUSMASTER_H

#include "qmodbus.h"
#include "qmodbuserror.h"
#include "qmodbusbits.h"
#include "qmodbusregisters.h"

namespace Modbus
{
    class QModbusMaster : public QObject
    {
        Q_OBJECT
    public:
        QModbusMaster (const char *ip, int port, QObject* parent = 0);
        QModbusMaster (const char *device, int baud, char parity, int dataBit, int stopBit, QObject* parent = 0);
        QModbusMaster (const char *node, const char *service, QObject* parent = 0);
        ~QModbusMaster ();
        QModbusError & lastError ();
        void setSlave (int slave);
        void setBroadcast ();
        bool connect();
        void close ();
        void flush ();

        //Context setters and getters
        void getByteTimeout (long *sec, long *usec);
        void setByteTimeout (long sec, long usec);
        void setDebug (bool debug);
        void setErrorRecovery (errorRecoveryMode errorRecovery);
        int getHeaderLength (void);
        void getResponseTimeout (long *sec, long *usec);
        void setResponseTimeout (long sec, long usec);

        // Read data
        void readBits (QModbusBits &bits);
        void readInputBits (QModbusBits &bits);
        void readRegisters (QModbusRegisters &regs);
        void readInputRegisters (QModbusRegisters &regs);
        void reportSlaveId (quint8 *dest);

        // Write data
        void writeBit (QModbusBits &bit);
        void writeRegister (QModbusRegisters &reg);
        void writeBits (QModbusBits &bits);
        void writeRegisters (QModbusRegisters &regs);

        // Write and read data
        void writeAndReadRegisters (QModbusRegisters &writeRegs, QModbusRegisters &readRegs);

        // Raw data //注意此方式有CRC校验
        void writeRawData(QByteArray &raw);
        void readRawData(QByteArray &raw);
        void writeAndReadRawData(QByteArray &wr,QByteArray &rd);

        // Raw No Crc
        void writeRawDataWithOutCRC(QByteArray &raw);
        void readRawDataWithOutCRC(QByteArray &raw);
        void readRawDataLength(QByteArray &raw,quint8 length);
        QString readRawDataLength(quint8 length);
        void writeAndReadRawDataWithOutCRC(QByteArray &wr,QByteArray &rd);        
        QString readRawDataWithOutCRC();

    private:
        void *ctx;
        QModbusError modbusError;
        bool connected;
        //
        inline void checkContext (void *ctx);
        inline void checkOperationsReturnValue (int returnValue);
    public:
        //Q Qt4.8.7版本  release 版本 inline错误 未定义 debug版本正常
        //todo
        bool checkConnection ();
    };
};

#endif // QMODBUSMASTER_H

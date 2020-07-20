#include "modbus.h"
#include "qmodbusmaster.h"
#include "qmodbuserror.h"
#include "qmodbusbits.h"
#include "qmodbusregisters.h"

#include <QDebug>
#include <qmath.h>
#include <QThread>

using namespace Modbus;

QModbusMaster::QModbusMaster (const char *ip, int port, QObject* parent) : QObject (parent)
{
    connected = false;
    ctx = (modbus_t *) modbus_new_tcp (ip, port);
    checkContext ((modbus_t *) ctx);
}

QModbusMaster::QModbusMaster (const char *device, int baud, char parity, int dataBit, int stopBit, QObject* parent) : QObject (parent)
{
#ifdef Q_OS_WIN32
    QString devstr(device);
    // is it a serial port in the range COM1 .. COM9?
    if ( devstr.startsWith( "COM1" ) )
    {
        // use windows communication device name "\\.\COMn"
        devstr.clear();
        devstr = QString("\\\\.\\").append(device);
        ctx = (modbus_t *) modbus_new_rtu ( devstr.toStdString().c_str(), baud, parity, dataBit, stopBit);
    }
    else
#endif
        ctx = (modbus_t *) modbus_new_rtu (device, baud, parity, dataBit, stopBit);

    checkContext ((modbus_t *) ctx);
}

QModbusMaster::QModbusMaster (const char *node, const char *service, QObject* parent) : QObject (parent)
{
    ctx = (modbus_t *) modbus_new_tcp_pi (node, service);
    checkContext ((modbus_t *) ctx);
}

QModbusMaster::~QModbusMaster ()
{
    close ();
    modbus_free ((modbus_t *) ctx);
}

QModbusError & QModbusMaster::lastError ()
{
    return modbusError;
}

void QModbusMaster::setSlave (int slave)
{
    if (modbus_set_slave ((modbus_t *) ctx, slave) != 0) {
        modbusError.set (errno);
        qErrnoWarning (errno, modbus_strerror (errno));
    }
    else {
        modbusError.clear ();
    }
}

void QModbusMaster::setBroadcast ()
{
    if (modbus_set_slave((modbus_t *) ctx, MODBUS_BROADCAST_ADDRESS) != 0) {
        modbusError.set (errno);
        qErrnoWarning (errno, modbus_strerror (errno));
    }
    else {
        modbusError.clear ();
    }
}

bool QModbusMaster::connect ()
{
    if (modbus_connect ((modbus_t *) ctx) != 0) {
        modbusError.set (errno);
        qErrnoWarning (errno, modbus_strerror (errno));
        connected = false;
    }
    else {
        modbusError.clear ();
        connected = true;
    }
    return connected;
}

void QModbusMaster::close()
{
    modbus_close ((modbus_t *) ctx);
}

void QModbusMaster::flush ()
{
    if (modbus_flush ((modbus_t *) ctx) != 0) {
        modbusError.set (errno);
        qErrnoWarning (errno, modbus_strerror (errno));
    }
    else {
        modbusError.clear ();
    }
}

void QModbusMaster::getByteTimeout (long *sec, long *usec)
{
    struct timeval timeout;
    modbus_get_byte_timeout ((modbus_t *) ctx, &timeout);
    *sec = timeout.tv_sec;
    *usec = timeout.tv_usec;
}

void QModbusMaster::setByteTimeout (long sec, long usec)
{
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
    modbus_set_byte_timeout ((modbus_t *) ctx, &timeout);
}

void QModbusMaster::setDebug (bool debug)
{
    modbus_set_debug ((modbus_t *) ctx, (int)debug);
}

void QModbusMaster::setErrorRecovery (errorRecoveryMode errorRecovery)
{
    if (!modbus_set_error_recovery ((modbus_t *) ctx, (modbus_error_recovery_mode)errorRecovery)) {
        modbusError.set (errno);
        qErrnoWarning (errno, modbus_strerror (errno));
    }
    else {
        modbusError.clear ();
    }
}

int QModbusMaster::getHeaderLength (void)
{
    return modbus_get_header_length ((modbus_t *) ctx);
}

void QModbusMaster::getResponseTimeout (long *sec, long *usec)
{
    struct timeval timeout;
    modbus_get_response_timeout ((modbus_t *) ctx, &timeout);
    *sec = timeout.tv_sec;
    *usec = timeout.tv_usec;
}

void QModbusMaster::setResponseTimeout (long sec, long usec)
{
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
    modbus_set_response_timeout ((modbus_t *) ctx, &timeout);
}

void QModbusMaster::readBits (QModbusBits &bits)
{
    if(checkConnection()) {
        checkOperationsReturnValue (modbus_read_bits ((modbus_t *) ctx, bits.addr, bits.size (), (quint8 *)bits.data ()));
    }
}

void QModbusMaster::readInputBits (QModbusBits &bits)
{
    if(checkConnection()) {
        checkOperationsReturnValue (modbus_read_input_bits ((modbus_t *) ctx, bits.addr, bits.size (), (quint8 *)bits.data ()));
    }
}

void QModbusMaster::readRegisters (QModbusRegisters &regs)
{
    if(checkConnection()) {
        checkOperationsReturnValue (modbus_read_registers ((modbus_t *) ctx, regs.addr, regs.size (), regs.data ()));
    }
}

void QModbusMaster::readInputRegisters (QModbusRegisters &regs)
{
    if(checkConnection()) {
        checkOperationsReturnValue (modbus_read_input_registers ((modbus_t *) ctx, regs.addr, regs.size (), regs.data ()));
    }
}

void QModbusMaster::reportSlaveId (quint8 *dest)
{
    if(checkConnection()) {
        checkOperationsReturnValue (modbus_report_slave_id ((modbus_t *) ctx, dest));
    }
}

void QModbusMaster::writeBit (QModbusBits &bit)
{
    if(checkConnection()) {
        checkOperationsReturnValue (modbus_write_bit ((modbus_t *) ctx, bit.addr, bit.data ()[0]));
    }
}

void QModbusMaster::writeRegister (QModbusRegisters &reg)
{
    if(checkConnection()) {
        checkOperationsReturnValue (modbus_write_register ((modbus_t *) ctx, reg.addr, (unsigned int)(reg.data ()[0])));
    }
}

void QModbusMaster::writeBits (QModbusBits &bits)
{
    if(checkConnection()) {
        checkOperationsReturnValue (modbus_write_bits ((modbus_t *) ctx, bits.addr, bits.size(), (quint8 *)bits.data ()));
    }
}

void QModbusMaster::writeRegisters (QModbusRegisters &regs)
{
    if(checkConnection()) {
        checkOperationsReturnValue (modbus_write_registers ((modbus_t *) ctx, regs.addr, regs.size(), regs.data ()));
    }
}

void QModbusMaster::writeAndReadRegisters (QModbusRegisters &writeRegs, QModbusRegisters &readRegs)
{
    if(checkConnection()) {
        checkOperationsReturnValue (modbus_write_and_read_registers ((modbus_t *) ctx, writeRegs.addr, writeRegs.size (), writeRegs.data (), readRegs.addr, readRegs.size (), readRegs.data ()));
    }
}

void QModbusMaster::writeRawData(QByteArray &raw)
{
    int length = modbus_send_raw_request((modbus_t *)ctx,(uint8_t*)raw.data(),raw.size());
    qDebug() << __FUNCTION__ << raw.size();
    if(length != raw.size())
        modbusError.set (RAW_WRITE_ENO);
    else
        modbusError.clear();
}

void QModbusMaster::readRawData(QByteArray &raw)
{
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    int req_length = modbus_receive_confirmation((modbus_t *) ctx, rsp);
    if(req_length > 0)
        raw.append((const char*)rsp,req_length);
}

void QModbusMaster::writeAndReadRawData(QByteArray &wr, QByteArray &rd)
{
    int length = modbus_send_raw_request((modbus_t *)ctx,(uint8_t*)wr.data(),wr.size());
    if(length != wr.size())
        modbusError.set (RAW_WRITE_ENO);
    else
        modbusError.clear();

    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    int req_length = modbus_receive_confirmation((modbus_t *) ctx, rsp);
    if(req_length > 0)
        rd.append((const char*)rsp,req_length);
}

void QModbusMaster::writeRawDataWithOutCRC(QByteArray &raw)
{
    int length = send_msg_withoutCRC((modbus_t *)ctx,(uint8_t*)raw.data(),raw.size());
    if(length != raw.size())
        modbusError.set (RAW_WRITE_ENO);
    else
        modbusError.clear();
}

void QModbusMaster::readRawDataWithOutCRC(QByteArray &raw)
{
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    int req_length = receive_msg_withoutCRC((modbus_t *) ctx, rsp);
    if(req_length > 0)
        raw.append((const char*)rsp,req_length);

    qDebug() << raw.toHex();

}

QString QModbusMaster::readRawDataWithOutCRC(void)
{
    QByteArray raw;
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    int req_length = receive_msg_withoutCRC((modbus_t *) ctx, rsp);
    if(req_length > 0)
        raw.append((const char*)rsp,req_length);

    qDebug() << __FUNCTION__ << req_length << raw.toHex();
    return raw.toHex();

}
QString QModbusMaster::readRawDataLength(quint8 length)
{
    QByteArray raw;
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
   // length = qMin(length,(quint8)MODBUS_RTU_MAX_ADU_LENGTH);
    int req_length = receive_msg_length((modbus_t *) ctx, rsp,length);
    if(req_length > 0){
        for(int i=0;i< req_length;i++){
            raw.append(*(rsp+i));
        }
        //raw.append((const char*)rsp,req_length);
    }

    qDebug() << "读出的数据" <<raw.toHex() << QThread::currentThread();
    return raw.toHex();
}

void QModbusMaster::readRawDataLength(QByteArray &raw, quint8 length)
{
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
   // length = qMin(length,(quint8)MODBUS_RTU_MAX_ADU_LENGTH);
    int req_length = receive_msg_length((modbus_t *) ctx, rsp,length);
    if(req_length > 0){
        raw.append((const char*)rsp,req_length);
    }
}

void QModbusMaster::writeAndReadRawDataWithOutCRC(QByteArray &wr, QByteArray &rd)
{
    int length = send_msg_withoutCRC((modbus_t *)ctx,(uint8_t*)wr.data(),wr.size());
    if(length != wr.size())
        modbusError.set (RAW_WRITE_ENO);
    else
        modbusError.clear();

    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    int req_length = receive_msg_withoutCRC((modbus_t *) ctx, rsp);
    if(req_length > 0)
        rd.append((const char*)rsp,req_length);
}

// private
void QModbusMaster::checkContext (void *ctx)
{
    if (ctx == NULL) {
        modbusError.set (errno);
        qErrnoWarning (errno, modbus_strerror (errno));
    }
    else {
        modbusError.clear ();
    }
}

bool QModbusMaster::checkConnection ()
{
    if (!connected) {
        modbusError.set (NOT_CONNECTED_ENO);
    }
    else {
        modbusError.clear ();
    }
    return connected;
}

void QModbusMaster::checkOperationsReturnValue (int operationsReturnValue)
{
    if (operationsReturnValue == -1) {
        modbusError.set (errno);
        qErrnoWarning (errno, modbus_strerror (errno));
    }
    else {
        modbusError.clear ();
    }
}

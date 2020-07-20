#ifndef QMODBUSERROR_H
#define QMODBUSERROR_H

#include <QObject>

#include "qmodbus.h"

#define MODBUS_ENOBASE 112345678
#define OUT_OF_RANGE_ENO (MODBUS_ENOBASE-1)
#define OUT_OF_RANGE_MSG "Register index out of range"
#define NOT_CONNECTED_ENO (MODBUS_ENOBASE-2)
#define NOT_CONNECTED_MSG "Not connected to slave"
#define RAW_WRITE_ENO     (MODBUS_ENOBASE-3)
#define RAW_WRITE_MSG     "Raw Data send incomplete"

class QString;

namespace Modbus
{
    class QModbusError
    {
        friend class QModbusMaster;
        friend class QModbusBits;
        friend class QModbusRegisters;
    public:
        QModbusError ();
        QModbusError (const QModbusError & other);
        bool isValid ();
        QString message ();
    private:
        void set (int errnum);
        void clear ();
        int error;
        bool valid;
    };
};

#endif // QMODBUSERROR_H

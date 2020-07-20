#ifndef QMODBUSBITS_H
#define QMODBUSBITS_H

#include "qmodbus.h"
#include "qmodbuserror.h"

#include <QVector>
#include <QDebug>

namespace Modbus
{
    class QModbusBits : public QVector <bool>
    {
        friend class QModbusMaster;
    public:
        QModbusBits (unsigned int address = 0, unsigned int number = 1);
        ~QModbusBits(){}
        bool getValue (unsigned int index);
        void setValue (unsigned int index, bool value);
        unsigned int getNumber (void);
        void setNumber (unsigned int number);
        unsigned int getAddress (void);
        void setAddress (unsigned int address);
    private:
        unsigned int addr;
        QModbusError modbusError;
    };
};

#endif // QMODBUSBITS_H

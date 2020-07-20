#include "modbus_backend.h"
#include "qapphelper.h"
modbus_backend::modbus_backend(QObject *parent) :
    QObject(parent)
{
    backendInit();
}

modbus_backend::modbus_backend(QString port, qint16 baud, char parity, qint8 databits, qint8 stopbits,bool debugmod, QObject *parent)
{
    backendInit();
    m_mbPortOk =  init(port,baud,parity,databits,stopbits,debugmod);
}

modbus_backend::~modbus_backend()
{
    disconnect(this);
}

bool modbus_backend::init(QString port,qint16 baud,char parity,qint8 databits,qint8 stopbits,bool debugmod)
{
    this->m_port = port;
    this->m_baud = baud;
    this->m_parity = parity;
    this->m_databits = databits;
    this->m_stopbits = stopbits;

    QByteArray qba_port = m_port.toAscii();
    const char *cc_port = qba_port.constData();
    m_mb = modbus_new_rtu(cc_port, m_baud, m_parity, m_databits, m_stopbits);   //相同的端口只能同时打开一个
    modbus_set_debug(m_mb,debugmod);
    modbus_set_slave(m_mb,m_slaveaddr);  //设置modbus从机地址
    int mb_res = modbus_connect(m_mb);


    if(mb_res == -1)
    {
        QAppHelper::ShowMessageBoxError(QString("SerialPort(%1) is open failed!").arg(m_port));
    }

    return (mb_res!=-1 ? true:false);
}

void modbus_backend::setSlaveAddr(qint8 addr)
{
    m_slaveaddr = addr;
    modbus_set_slave(m_mb, m_slaveaddr);  //设置modbus从机地址
}

bool modbus_backend::isAvailable()
{
    return m_mbPortOk;
}

void modbus_backend::setDebugMod(bool mod)
{
    modbus_set_debug(m_mb,mod);
}

//attention: remaiining time 用int 表示 可能会溢出 不过由于逻辑功能限制 此处未进行处理
void modbus_backend::setResponseTimeout(long sec, long usec)
{
    m_tout.tv_sec = sec;
    m_tout.tv_usec = usec;
}

timeval modbus_backend::getTimeout()
{
    return m_tout;
}


//读保持寄存器
int modbus_backend::readRegisters(int addr, int nb)
{
    m_qv_readregs.resize(nb);//attention 注意nb表示寄存器个数 并非字节
    int regs = modbus_read_registers(m_mb,addr,nb,(uint16_t*)m_qv_readregs.data());
    qDebug() << __FUNCTION__ << nb << regs;
    qDebug() << m_qv_readregs << m_qv_readregs.size() ;
}

int modbus_backend::writeRegister(int reg_addr, int value)
{
    modbus_write_register(m_mb,reg_addr,value);
}





//初始化类本身
void modbus_backend::backendInit()
{
    setResponseTimeout();
    m_mbPortOk = false;
    m_slaveaddr = 0x01;
}




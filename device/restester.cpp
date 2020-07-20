#include "restester.h"
#include "qapphelper.h"
#include "voltageset.h"

#define ACKDELAY 200

ResTester::ResTester(const QString &port, int baud, char pairty, int databits, int stopbits, QObject *parent)
    : QObject(parent)
{
    modbusMaster = new QModbusMaster(port.toStdString().c_str(),baud,pairty,databits,stopbits);
    modbusMaster->connect();
    m_hasError = modbusMaster->checkConnection();
    modbusMaster->setDebug(true);
    //    modbusMaster->setResponseTimeout(0,50*1000);
    //    modbusMaster->setByteTimeout(0,100);

    //modbusMaster->setSlave(3);
}

//发送准备自检命令
//设备主动回复 0x40 0x02 0x00 0x07(0x01)
bool ResTester::powerOnSelfTestCMD()
{
    QByteArray wb;  
    QString rbstr;
    //调试工具对 0x7f处理有异常 调试阶段使用0x33代替
    {
        qint8 array[] = {0x40,0x01,0x00,0x7f};
        wb.append((const char*)array,4);
        modbusMaster->writeRawDataWithOutCRC(wb);
        qDebug() << __FUNCTION__  << "发送的数据：" << wb.toHex();
        QAppHelper::delayms(ACKDELAY);
        rbstr = modbusMaster->readRawDataLength(4);
        qDebug() << __FUNCTION__  << "回读的数据：" << rbstr;
    }
    QByteArray tarray;
    qint8 array[] = {0x40,0x02,0x00};
    tarray.append((const char*)array,3);
    qint8 res = readDataStringAnylasis(rbstr,tarray.toHex());

    qDebug() << __FUNCTION__ << "测试仪返回数据" << rbstr;
    if(res == 0x07)
    {
        qDebug() << __FUNCTION__  << "电阻测试仪自检通过";
        return true;
    }else
    {
        qDebug() << __FUNCTION__  << "电阻测试仪自检失败";
        return false;
    }
}

//通道自检命令
void ResTester::setPreChanelTestMode()//通道自检命令
{
    qDebug() << "发送通道自检";
    sendCurrentMode((char)0x01);
    QAppHelper::delayms(ACKDELAY);
}

qint8 ResTester::setTestMode()
{
    sendCurrentMode((char)0x03); //发送测试命令
    QAppHelper::delayms(3000);
    QString rb =  (modbusMaster->readRawDataLength(8));

    rb.append(modbusMaster->readRawDataLength(20));

    // qDebug() << __FUNCTION__ << rb.toHex();
    QByteArray tarray;
    qint8 array[] = {0x40,0x02,0x10};
    tarray.append((const char*)array,3);
    qint8 res = readDataStringAnylasis(rb,tarray.toHex());
    qDebug() << "----------:"<< __FUNCTION__ <<rb << res;
    return res;
}


void ResTester::setWorkMode(int mode)
{
    sendCurrentMode((char)mode); //发送测试命令
}

bool ResTester::hasError(){
    return m_hasError;
}

//设置测试通道号  4001x
//设备回复  40 01 01 x  40 02 01 xx
void ResTester::setCurrentChannel(int num)
{
    QByteArray wb;
    qint8 array[] = {0x40,0x01,0x01};
    wb.append((const char*)array,3);
    if(num < 31 && num > 0)
        wb.append((qint8)num);
    else
        wb.append((char)0);
    modbusMaster->writeRawDataWithOutCRC(wb);

    QString rb =  modbusMaster->readRawDataLength(4);
    //按照流程图设备回复的数据丢弃 不做处理
}

//设置绝缘测试电压 400103
void ResTester::setCurrentVoltage(int vol)
{
    int vol_ = VoltageSet::bcd2decimal(vol);
    qDebug() << __FUNCTION__ << "设置电压:" << vol_ << "*10 VDC" << "bcdcode" << QString::number(vol,16);
    QByteArray wb;
    qint8 array[] = {0x40,0x01,0x03};
    wb.append((const char*)array,3);

    if(vol < 0x51 && vol > 0)
        wb.append((quint8)vol);
    else
        wb.append((char)0);
    modbusMaster->writeRawDataWithOutCRC(wb);
}

//设置工作模式，并回读数据
//返回值为错误代码  读出的测量值在 res参数
//注意绝缘模式时候 voltage 参数不可省略
qint8 ResTester::runTest(char mod,bool *isCredible, float *res,int voltage)
{
    qDebug()<< __FUNCTION__ << "mod " << mod;
    *isCredible = false;
    *res = 0;
    qint8 ret = 0 ;//返回值


    if(mod == (char)CMD_INS)
    {
        setCurrentVoltage(voltage);
    }
    sendCurrentMode(mod);

    QString rb;
    qint8 reg0207 = 0;
    {
        sendCurrentMode((char)0x03); //发送测试命令
        QAppHelper::delayms(500);
        rb =  (modbusMaster->readRawDataLength(8));//读出28个字节
        qDebug() << rb;
        QString rbc = (modbusMaster->readRawDataLength(4));//读出28个字节
        qDebug() << rbc;
        rb.append(rbc);
        {
            QString rbc = (modbusMaster->readRawDataLength(4));//读出28个字节
            qDebug() << rbc;
            rb.append(rbc);
        }
        {
            QString rbc = (modbusMaster->readRawDataLength(4));//读出28个字节
            qDebug() << rbc;
            rb.append(rbc);
        }
        {
            QString rbc = (modbusMaster->readRawDataLength(4));//读出28个字节
            qDebug() << rbc;
            rb.append(rbc);
        }
        {
            QString rbc = (modbusMaster->readRawDataLength(4));//读出28个字节
            qDebug() << rbc;
            rb.append(rbc);
        }

        qDebug() << "recv msg: " << rb;
        QByteArray tarray;
        tarray.resize(0);
        qint8 array[] = {0x40,0x02,0x07};
        tarray.append((const char*)array,3);
        qDebug() << "reg read "  << rb;
        reg0207 = readDataStringAnylasis(rb,tarray.toHex());
        qDebug() << "reg0207 " << reg0207 << rb;
    }
    if(reg0207 != 1)
    {
        if(reg0207 !=3)
        {
            return -6; //线路干扰
        }else
        {
            //自检失败 提示是否继续测试
            *isCredible = false;
            ret = -5;
        }
    }else
    {
        *isCredible = true;
        ret = 0;
    }

    QByteArray tarray;
    QByteArray resbytearray; //暂存结果的数组
    {
        qint8 array0[] = {0x40,0x02,0x03};
        tarray.resize(0);
        tarray.append((const char*)array0,3);
        resbytearray.append(readDataStringAnylasis(rb,tarray.toHex()));

        qint8 array1[] = {0x40,0x02,0x04};
        tarray.resize(0);
        tarray.append((const char*)array1,3);
        resbytearray.append(readDataStringAnylasis(rb,tarray.toHex()));

        qint8 array2[] = {0x40,0x02,0x05};
        tarray.resize(0);
        tarray.append((const char*)array2,3);
        resbytearray.append(readDataStringAnylasis(rb,tarray.toHex()));

        qint8 array3[] = {0x40,0x02,0x06};
        tarray.resize(0);
        tarray.append((const char*)array3,3);
        resbytearray.append(readDataStringAnylasis(rb,tarray.toHex()));

        //此处值 传出默认为欧姆
        union {
            float floatdat;
            char byte[4];
        }float_byte;
        float_byte.byte[0] = resbytearray.at(0);
        float_byte.byte[1] = resbytearray.at(1);
        float_byte.byte[2] = resbytearray.at(2);
        float_byte.byte[3] = resbytearray.at(3);
        *res = float_byte.floatdat;
        //此处值 传出默认为欧姆

        qDebug() << "!!!!!!" <<__FUNCTION__ << QString("测量结果(03 04 05 06 寄存器的值)")
                 << resbytearray.toHex() << *res << QString("Ω");
    }

    return ret;
}

//设置测试仪工作模式  400102

//40010207  40 02 10 07 40 02 02 07
void ResTester::sendCurrentMode(char mod)
{
    QByteArray wb;
    qint8 array[] = {0x40,0x01,0x02};
    wb.append((const char*)array,3);
    wb.append((char)mod);
    modbusMaster->writeRawDataWithOutCRC(wb);

    if(mod == 0x03) return;
    QAppHelper::delayms(500);
    QString rb = modbusMaster->readRawDataLength(8);
    if(0) //按照流程图设备回复的数据丢弃
    {
        QByteArray tarray;
        qint8 array[] = {0x40,0x02,0x07};
        tarray.append((const char*)array,3);
        readDataStringAnylasis(rb,tarray.toHex());
    }
}

//在a中查找 b 并且将找到的后一个值返回
qint8 ResTester::readDataAnylasis_ (QByteArray a,QByteArray b)
{
    //qDebug() << __FUNCTION__ << a.toHex() <<" indexOf " <<b.toHex();
    if(a.isEmpty())
        return -1;
    int pos = a.indexOf(b);
    QByteArray tarr = a.right(a.size() - pos);
    //qDebug() << __FUNCTION__ << "pos" << pos << "tarr " << tarr.toHex();
    qint8 dat = -1;
    if(!tarr.isEmpty() && tarr.size() >= 4 && pos >= 0)
    {
        dat = tarr.at(3);
    }
    return dat;
}

qint8 ResTester::readDataStringAnylasis(QString a, QString b)
{
    qint8 dat = -1;

    if(a.isEmpty())
        return -1;
    int pos = a.indexOf(b);
    if(pos < 0) return -1;

    pos+=b.length();
    QString datstr = "";
    if(a.length() > pos+1)
    {
        datstr.append(a.at(pos));
        datstr.append(a.at(pos+1));
        bool b;
        dat = datstr.toInt(&b,16);
    }
    //qDebug() << "ansys" << a << ":"<< b<< datstr <<dat;
    return dat;
}



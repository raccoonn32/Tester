#include "gpio.h"

#include <QDebug>

#if defined(IMX_GPIO)
gpio::gpio()
{
    m_fd = -1;
    m_fd = open(DEV_GPIO, O_RDWR);
    if (m_fd < 0) {
        printf("Error device open fail! %d\n", m_fd);
    }
}

gpio::~gpio()
{
    if(m_fd >= 0)
        close(m_fd);
}


bool gpio::getGpio(int pin,bool defaultVal)
{
    m_arg.pin = pin;
    m_arg.data = defaultVal; //先设置默认值
    m_arg.usepullup = defaultVal;
    if(m_fd > -1)
    {
        ioctl(m_fd,IOCTL_GPIO_SETINPUT,&m_arg);
        ioctl(m_fd, IOCTL_GPIO_GETVALUE, &m_arg);
        //qDebug() << m_arg.pin << m_arg.data << m_arg.usepullup;
        return (!(m_arg.data == 0));
    }
    return 0;
}

void gpio::setGpio(int pin, bool b,bool pullup)
{
    m_arg.pin = pin;
    m_arg.data = (b?1:0) ;
    m_arg.usepullup = (pullup?1:0);
    ioctl(m_fd, IOCTL_GPIO_SETOUTPUT, &m_arg);
}

int gpio::getdevfd()
{
   return m_fd;
}

#endif

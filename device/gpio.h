#ifndef GPIO_H
#define GPIO_H

#include <QtGlobal>

#if !defined(Q_OS_WIN32)
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#endif

#define DEV_GPIO "/dev/user_gpio"

#define GPIO_IOC_MAGIC   'G'
#define IOCTL_GPIO_SETOUTPUT              _IOW(GPIO_IOC_MAGIC, 0, int)
#define IOCTL_GPIO_SETINPUT               _IOW(GPIO_IOC_MAGIC, 1, int)
#define IOCTL_GPIO_SETVALUE               _IOW(GPIO_IOC_MAGIC, 2, int)
#define IOCTL_GPIO_GETVALUE    		      _IOR(GPIO_IOC_MAGIC, 3, int)

#define GPIO(a,b)  (int)((a-1)*0x20+b)

//#define GPIO_BRI GPIO(1,18)
//#define GPIO_INS GPIO(7,7)

#define GPIO_SEL GPIO(1,16)     //J25 pin11
#define GPIO_TEL GPIO(1,18)     //J25 pin12

#define LED1 GPIO(7,7)
#define LED2 GPIO(7,2)
#define LED3 GPIO(7,8)
#define LED4 GPIO(7,3)

typedef struct {
    int pin;
    int data;
    int usepullup;
}gpio_arg;

#if !defined(IMX_GPIO)
class gpio
{
public:
    gpio(){}
    ~gpio(){}

    bool getGpio(int pin,bool b=0){}
    void setGpio(int pin,bool b,bool x){}

private:
    gpio_arg m_arg;
    int m_fd;
};
#else
class gpio
{
public:
    gpio();
    ~gpio();

    bool getGpio(int pin,bool defaultVal = 0);
    void setGpio(int pin, bool b, bool pullup = 1);
    int getdevfd();
private:
    gpio_arg m_arg;
    int m_fd;
};

#endif

#endif // GPIO_H

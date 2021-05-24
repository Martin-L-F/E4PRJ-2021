#include "UART.hpp"
#include <stdlib.h>

UART::UART()
{
    //Setup for the node: baud: 9600, no echo, raw mode
    system("stty -F /dev/ttyACM0 9600");
    system("stty -F /dev/ttyACM0 -echo");
    system("stty -F /dev/ttyACM0 raw");

    fd_ = open("/dev/ttyACM0", O_RDWR, S_IRUSR | S_IWUSR);
    if( fd_ == -1)
        printf("Failed to open ttyACM0 with err: %s", strerror(errno));
}

UART::~UART()
{
    close(fd_);
}

void UART::writeMsg(char *msg)
{
    int ret;

    ret = write(fd_, msg, strlen(msg)+1);
    if( ret == -1)
        printf("Failed to write ttyACM0 with err: %s", strerror(errno));
}

int UART::readMsg(char *msg, int size) const
{
    int ret;

    ret = read(fd_, msg, size);
    if( ret == -1)
        printf("Failed to read ttyACM0 with err: %s", strerror(errno));

    return ret;
}
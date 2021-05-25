#include "UART.hpp"
#include <stdlib.h>

UART::UART()
{
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

void UART::writeMsg(uint8_t *msg, uint8_t SIZE)
{
    int ret;

    ret = write(fd_, msg, SIZE);
    if( ret == -1)
        printf("Failed to write ttyACM0 with err: %s", strerror(errno));
}

uint8_t UART::readMsg(uint8_t *msg, uint8_t SIZE) const
{
    int ret;

    ret = read(fd_, msg, SIZE);
    if( ret == -1)
        printf("Failed to read ttyACM0 with err: %s", strerror(errno));

    return ret;
}





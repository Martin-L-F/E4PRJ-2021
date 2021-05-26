#include "UART.hpp"
#include <stdlib.h>
#include <iostream>
using namespace std;

UART::UART(string PSoC_ID)
{
    // system("stty -F /dev/ttyACM0 9600");
    // system("stty -F /dev/ttyACM0 -echo");
    // system("stty -F /dev/ttyACM0 raw");
    string full_PsoC_ID = "usb-Cypress_Semiconductor_Cypress_KitProg_" + PSoC_ID + "-if02";
    string USB_ID_path = "/dev/serial/by-id/";
    string stty_cmd = "stty -F ";
    string full_USB_ID_path = USB_ID_path + full_PsoC_ID;
    string full_stty_cmd = stty_cmd + USB_ID_path + full_PsoC_ID;

    string str_UART_baudrate = full_stty_cmd + " 9600";
    string str_UART_echo = full_stty_cmd + " -echo";
    string str_UART_raw = full_stty_cmd + " raw";

    //const char* UART_open = (USB_ID_path + full_PsoC_ID).data();
    const char* UART_open = full_USB_ID_path.c_str();
    const char* UART_baudrate = str_UART_baudrate.c_str();
    const char* UART_echo = str_UART_echo.c_str();
    const char* UART_raw = str_UART_raw.c_str();
    
    //printf("open path: %s\n", UART_open);
    //cout << "Cout open path" << UART_open << endl;

    system(UART_baudrate);
    system(UART_echo);
    system(UART_raw);

    fd_ = open(UART_open, O_RDWR, S_IRUSR | S_IWUSR);
    if( fd_ == -1)
        printf("Failed to open ttyACM0 with err: %s\n", strerror(errno));
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
        printf("Failed to write ttyACM0 with err: %s\n", strerror(errno));
}

uint8_t UART::readMsg(uint8_t *msg, uint8_t SIZE) const
{
    int ret;

    ret = read(fd_, msg, SIZE);
    if( ret == -1)
        printf("Failed to read ttyACM0 with err: %s\n", strerror(errno));

    return ret;
}





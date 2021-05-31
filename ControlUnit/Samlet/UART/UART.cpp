#include "UART.hpp"
#include <stdlib.h>
#include <iostream>

using namespace std;

UART::UART(string PSoC_ID)
{
    if (PSoC_ID != "0") {
        string full_PsoC_ID = "usb-Cypress_Semiconductor_Cypress_KitProg_" + PSoC_ID + "-if02";
        string USB_ID_path = "/dev/serial/by-id/";
        string stty_cmd = "stty -F ";
        string full_USB_ID_path = USB_ID_path + full_PsoC_ID;
        string full_stty_cmd = stty_cmd + USB_ID_path + full_PsoC_ID;

        string str_UART_baudrate = full_stty_cmd + " 9600";
        string str_UART_echo = full_stty_cmd + " -echo";
        string str_UART_raw = full_stty_cmd + " raw";

        const char* UART_open = full_USB_ID_path.c_str();
        const char* UART_baudrate = str_UART_baudrate.c_str();
        const char* UART_echo = str_UART_echo.c_str();
        const char* UART_raw = str_UART_raw.c_str();

        system(UART_baudrate);
        system(UART_echo);
        system(UART_raw);

        fd_ = open(UART_open, O_RDWR, S_IRUSR | S_IWUSR);
        if( fd_ == -1)
            printf("Failed to open ttyACM0 with err: %s\n", strerror(errno));

        ThreadUARTReceive = thread(bind(&UART::funcUARTReceive, this));
        ThreadUARTDispatcher = thread(bind(&UART::funcUARTDispatcher, this));
    }
    else
        fd_ = 0;
}

UART::~UART()
{
    if (fd_ != 0) {
        close(fd_);
        ThreadUARTReceive.detach();
        ThreadUARTDispatcher.detach();
    }
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

void UART::addCallback(function<void(unsigned, Message*)> cb) {
    callbackDispatcher_ = cb;
}

void UART::funcUARTReceive()
{
    int fd;
    
    //MsgQueue* MQobj = static_cast<MsgQueue*>(arg);
    //ControlUnit_Controller* controlObj = static_cast<ControlUnit_Controller*>(arg);

    if(debug_UART)
        cout << "Sender initiated..." << endl;

    fd = open("/dev/ttyACM0", O_RDWR, S_IRUSR | S_IWUSR);
    if( fd == -1)
        printf("Failed to open with err: %s", strerror(errno));

    uint8_t rx_mem[3] = {};
    uint8_t rx_buffer[3] = {};

    int n = 0;
    int r_numb = 0;

    while(1)
    {
        
        r_numb = read(fd, rx_buffer, 1);
        for (int i = 0; i < r_numb; i++)
        {
            rx_mem[(i+n)%3] = rx_buffer[i];
        }
        
        if(debug_UART)
        {
            cout << "Number of bytes read: " << r_numb << endl;
            for (int i = 0; i < r_numb; i++)
            {
                printf("Msg %i; %x\n", n, rx_buffer[i]);
            }
        }


        if (rx_mem[n] == 0xEF) {
            Rx_data* rxObj = new Rx_data(rx_mem);
            msgQueueObj.send(rx_mem[0], static_cast<Message*>(rxObj));
            if(debug_UART)
                cout << "Send msg to Queue" << endl;
  
            n = 0;
        }
        else
            n++;
    }
}
void UART::funcUARTDispatcher()
{
    if(debug_UART)
        cout << "Receiver initiated..." << endl;

    //ControlUnit_Controller* controlObj = static_cast<ControlUnit_Controller*>(arg);

    while(1)
    {
        unsigned long id;
        Message* msg = msgQueueObj.receive(id);
        callbackDispatcher_(id, msg);
        delete msg;
    }
}
#include "threads.hpp"

#include "Target_IF.hpp"
#include "UART/MsgQueue.hpp"
#include "ControlUnit_Controller.hpp"
#include <pthread.h>
#include <iostream>
using namespace std;

void *funcTargetIF_UARTReceive(void *arg)
{
    int fd;
    
    //MsgQueue* MQobj = static_cast<MsgQueue*>(arg);
    ControlUnit_Controller* controlObj = static_cast<ControlUnit_Controller*>(arg);

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
                printf("%x\n", rx_buffer[i]);
            }
        }


        if (rx_buffer[r_numb-1] == 0xEF) {
            Rx_data* rxObj = new Rx_data(rx_mem);
            controlObj->UART_msgQueueObj.send(ShotDetected, static_cast<Message*>(rxObj));
            if(debug_UART)
                cout << "Send msg to Queue" << endl;

                
            n = 0;
        }
        else
            n++;
    }

    pthread_exit(NULL);
}
void *funcTargetIF_Dispatcher(void* arg)
{
    if(debug_UART)
        cout << "Receiver initiated..." << endl;

    ControlUnit_Controller* controlObj = static_cast<ControlUnit_Controller*>(arg);

    while(1)
    {
        unsigned long id;
        Message* msg = controlObj->UART_msgQueueObj.receive(id);
        controlObj->TargetObj.getUARTobj()->UART_Dispatcher(id, msg);
        delete msg;
    }
    return nullptr;

}


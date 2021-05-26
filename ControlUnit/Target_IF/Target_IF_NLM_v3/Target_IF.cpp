#include "Target_IF.hpp"
#include "TargetIFThreads.hpp"

#include <iostream>
using namespace std;

Target_IF::Target_IF() : UARTobj_(Target_IF_PSOC_ID)
{

}

Target_IF::~Target_IF()
{

}

void Target_IF::startDetection(int numberOfPlayers)
{
    uint8_t msg[3];

    //snprintf(msg, BUF_SIZE, "%x%x%x\n", 0xD3, numberOfPlayers, 0xEF);
    msg[0] = 0xD3;
    msg[1] = numberOfPlayers;
    msg[2] = 0xEF;


    UARTobj_.writeMsg(msg, 3);
}

void Target_IF::stopDetection()
{
    uint8_t msg[2];

    //snprintf(msg, BUF_SIZE, "%x%x\n", 0xD7, 0xEF);

    msg[0] = 0xD7;
    msg[1] = 0xEF;

    UARTobj_.writeMsg(msg, 2);
}

void TargetIF_Dispatcher(unsigned id, Message* msg)
{
    switch(id) {
    case ShotDetected:
        updateScore(msg);
    break;
    default:
        if(debug_UART)
            cout << "unknown id" << endl;;
    }
    
    //if(rx_mem[0] == 0xD5)
    //targetObj->updateScore(rx_mem[1]);
}

int updateScore(Message* user) //Handler in TargetIF_Dispatcher
{

    Rx_data* rxObj = static_cast<Rx_data*>(user);

    //TO BE ADDED

    //if(debug_UART)
        printf("Player nr. %i get a point\n", rxObj->rx_mem_[1]);
        
    return 0;
}
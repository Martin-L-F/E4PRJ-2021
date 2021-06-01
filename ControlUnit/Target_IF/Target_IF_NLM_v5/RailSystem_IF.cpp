#include "RailSystem_IF.hpp"


RailSystem_IF::RailSystem_IF() : UARTobj_(RailSystem_IF_PSOC_ID)
{
    UARTobj_.addCallback(bind(&RailSystem_IF::UARTDispatcher, this, placeholders::_1, placeholders::_2));
}

RailSystem_IF::~RailSystem_IF()
{

}

void RailSystem_IF::startMoving(int difficulty)
{
    uint8_t msg[3];


    msg[0] = 0xD4;
    msg[1] = difficulty;
    msg[2] = 0xEF;

    UARTobj_.writeMsg(msg, 3);
}

void RailSystem_IF::resetPosition()
{
    uint8_t msg[2];


    msg[0] = 0xD1;
    msg[1] = 0xEF;
    
    UARTobj_.writeMsg(msg, 2);
}

void RailSystem_IF::stopMoving()
{
    uint8_t msg[2];

    msg[0] = 0xD6;
    msg[1] = 0xEF;

    UARTobj_.writeMsg(msg, 2);
}

void RailSystem_IF::UARTDispatcher(unsigned id, Message* msg)
{
    if(debug_UART)
        printf("RailSystem Dispatcher id: %x\n", id);

    switch(id) {
    case 0xD2:
         CalibrateDone(msg);
    break;
    default:
        if(debug_UART)
                cout << "Target Dispatcher: unknown id" << endl;
    }
}

int RailSystem_IF::CalibrateDone(Message* user) //Handler in TargetIF_Dispatcher
{

    //Rx_data* rxObj = static_cast<Rx_data*>(user);

    callbackControllerCalibrateComplete_();
        
    return 0;
}

void RailSystem_IF::addCallback(function<void()> cb) {
    callbackControllerCalibrateComplete_ = cb;
}

UART* RailSystem_IF::getUARTobj()
{
    return &UARTobj_;
}
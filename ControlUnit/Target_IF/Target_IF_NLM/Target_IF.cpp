#include "Target_IF.hpp"

Target_IF::Target_IF()
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

int Target_IF::updateScore(int user)
{
    //TO BE ADDED
    return 0;
}


#include "Target_IF.hpp"

Target_IF::Target_IF()
{

}

Target_IF::~Target_IF()
{

}

void Target_IF::startDetection(int numberOfPlayers)
{
    char msg[BUF_SIZE];

    snprintf(msg, BUF_SIZE, "%i%i%i", 0xD3, numberOfPlayers, 0xEF);

    UARTobj_.writeMsg(msg);
}

void Target_IF::stopDetection()
{
    char msg[BUF_SIZE];

    snprintf(msg, BUF_SIZE, "%i%i", 0xD7, 0xEF);

    UARTobj_.writeMsg(msg);
}

int Target_IF::updateScore(int user)
{
    //TO BE ADDED
    return 0;
}


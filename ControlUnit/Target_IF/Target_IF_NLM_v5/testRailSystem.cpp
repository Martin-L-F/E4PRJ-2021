#include "Target_IF.hpp"
#include "UART/MsgQueue.hpp"
#include "ControlUnit_Controller.hpp"

#include <pthread.h>
#include <iostream>
using namespace std;


int main()
{    
    ControlUnit_Controller controlObj;
    

    //Test
    printf("Program started\n");

    controlObj.RailSystemObj.resetPosition();
    printf("Start Calibration\n");
    controlObj.RailSystemObj.getUARTobj()->getThreadUARTREceive()->join();

    return 0;
}
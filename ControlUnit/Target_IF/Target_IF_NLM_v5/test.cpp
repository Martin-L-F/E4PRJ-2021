#include "Target_IF.hpp"
#include "UART/MsgQueue.hpp"
#include "threads.hpp"
#include "ControlUnit_Controller.hpp"

#include <pthread.h>
#include <iostream>
using namespace std;


int main()
{
    pthread_t UARTReceive, UARTDispatcher;
    
    ControlUnit_Controller controlObj;
    
  

    pthread_create(&UARTReceive, NULL, funcTargetIF_UARTReceive, &controlObj);
    pthread_create(&UARTDispatcher, NULL, funcTargetIF_Dispatcher, &controlObj);

        //Test
    printf("Program started\n");

    controlObj.TargetObj.startDetection(2);
    printf("Start detection\n");
    sleep(10);
    controlObj.TargetObj.stopDetection();
    printf("Stop detection\n");
    
    sleep(2);
    //pthread_join( UART_rx, NULL );

    return 0;
}
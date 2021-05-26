#include "Target_IF.hpp"
#include "MsgQueue.hpp"
#include "TargetIFThreads.hpp"

#include <pthread.h>
#include <iostream>
using namespace std;


int main()
{
    pthread_t UARTReceive, UARTDispatcher;

    MsgQueue msgQueueObj;
    Target_IF testObj;

    pthread_create(&UARTReceive, NULL, funcTargetIF_UARTReceive, &msgQueueObj);
    pthread_create(&UARTDispatcher, NULL, funcTargetIF_Dispatcher, &msgQueueObj);

    printf("Program started\n");

    testObj.startDetection(2);
    printf("Start detection\n");
    sleep(10);
    testObj.stopDetection();
    printf("Stop detection\n");

    sleep(2);
    //pthread_join( UART_rx, NULL );

    return 0;
}
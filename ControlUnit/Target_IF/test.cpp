#include "Target_IF.hpp"
#include <pthread.h>

void *funcUART_rx(void *arg)
{
    int fd;
    //ifstream
    fd = open("/dev/ttyACM0", O_RDWR, S_IRUSR | S_IWUSR);
    if( fd == -1)
        printf("Failed to open with err: %s", strerror(errno));
    
    while(1)
    {
        char rx_buffer[BUF_SIZE] = "";
        read(fd, rx_buffer, BUF_SIZE);
        printf("%s\n", rx_buffer);
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_t UART_rx;
    pthread_create(&UART_rx, NULL, funcUART_rx, NULL);

    printf("Program started\n");

    Target_IF testObj;

    testObj.startDetection(2);
    sleep(2);
    testObj.stopDetection();

    sleep(5);
    //pthread_join( UART_rx, NULL );

    return 0;
}
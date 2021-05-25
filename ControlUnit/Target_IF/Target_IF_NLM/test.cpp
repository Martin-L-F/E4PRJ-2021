#include "Target_IF.hpp"
#include <pthread.h>
#include <iostream>
using namespace std;

void *funcUART_rx(void *arg)
{
    int fd;
    //ifstream
    fd = open("/dev/ttyACM0", O_RDWR, S_IRUSR | S_IWUSR);
    if( fd == -1)
        printf("Failed to open with err: %s", strerror(errno));

    uint8_t rx_mem[3] = {};
    int n = 0;
    uint8_t rx_buffer[3] = {};

    int r_numb = 0;
    while(1)
    {
        
        r_numb = read(fd, rx_buffer, 1);
        for (int i = 0; i < r_numb; i++)
        {
            rx_mem[(i+n)%3] = rx_buffer[i];
        }
        
        // cout << "Number of bytes read: " << r_numb << endl;
        // for (int i = 0; i < r_numb; i++)
        // {
        //     printf("%x\n", rx_buffer[i]);
        // }
        
        
        if (rx_buffer[r_numb-1] == 0xEF) {
            if(rx_mem[0] == 0xD5)
                printf("Player nr. %x get a point\n", rx_mem[1]);
            
            n = 0;
        }
        else
            n++;
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
    printf("Start detection\n");
    sleep(10);
    testObj.stopDetection();
    printf("Stop detection\n");

    sleep(2);
    //pthread_join( UART_rx, NULL );

    return 0;
}
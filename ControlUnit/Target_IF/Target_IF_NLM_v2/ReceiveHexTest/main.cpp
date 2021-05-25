#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <iostream>

using namespace std;

void *funcUART_rx(void *arg)
{
    int fd;
    //ifstream
    fd = open("/dev/ttyACM0", O_RDWR, S_IRUSR | S_IWUSR);
    if( fd == -1)
        printf("Failed to open with err: %s", strerror(errno));


    while(1)
    {
        //char rx[2] = {};
        uint8_t rx[3] = {};
        rx[0] = 0x4;
        rx[1] = 0x5;
        //read(fd, rx, 2);
        write(fd, rx, 3);
        //rx_buffer[1] = '\0';
        //if (rx[1] == 0xEF || rx[2] == 0xEF)
            //printf("%x %x %x\n", rx[0], rx[1], rx[2]);
        //printf("%i %i\n", rx[0], rx[1]);
        //printf("%s\n", rx);
        //if (rx[0] == 0x24 || rx[1] == '$' || rx[1] == 'r' || rx[0] == 0xE || rx[0] == '$')
            //cout << rx[0] << rx[1] << endl;
        //cout << endl;

        sleep(2); 
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_t UART_rx;
    printf("Program started\n");
    pthread_create(&UART_rx, NULL, funcUART_rx, NULL);



    pthread_join( UART_rx, NULL );

    return 0;
}
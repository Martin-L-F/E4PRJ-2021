#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <fstream>
#define BUF_SIZE 32

void *funcUART_rx(void *arg)
{
    int fd;
    //ifstream
    fd = open("/dev/ttyACM0", O_RDWR, S_IRUSR | S_IWUSR);
    if( fd == -1)
        printf("Failed to open with err: %s", strerror(errno));
    //char rx_buffer[BUF_SIZE] {};

    while(1)
    {
        printf("Reading now\n");
        char rx_buffer[BUF_SIZE] = "";
        
        read(fd, rx_buffer, BUF_SIZE);
        printf("%s", rx_buffer);

        //snprintf(rx_buffer, 0, "");
        //memset(rx_buffer, 0, BUF_SIZE);
    }

    pthread_exit(NULL);
}

int main()
{
    int err, fd;
    pthread_t UART_rx;

    printf("Program started\n");
    
    fd = open("/dev/ttyACM0", O_RDWR, S_IRUSR | S_IWUSR);
    if( fd == -1) {
        printf("Failed to open with err: %s", strerror(errno));
        return -1;
    }

    pthread_create(&UART_rx, NULL, funcUART_rx, NULL);

    sleep(1);
    err = write(fd, "Hello_PSoC\n", 11);
    sleep(1);
    err = write(fd, "Message\n", 8);

    pthread_join( UART_rx, NULL );

    return 0;
}
//Compile m. linjen:
//arm-rpizw-gcc -o UART UART.cpp
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

typedef  unsigned int uint32_t;

#define BCM_2835_PERIPH_BASE    0x20000000      // Peripherals base addr on BCM2835
#define BCM_2835_PERIPH_SIZE    0x01000000      // Peripherals size
#define GPIO_OFFSET             0x200000
#define UART_OFFSET             0x20100
/* BCM2835 p. 91 ff. */
#define GPFSEL1                 0x04            //GPFSEL: define operation of the general-purpose I/O, GPFSEL1 Address offset for GPIO 10-19, 32-bit 
#define UART_DR                 0x0             //DR: Data Register
#define UART_FR                 0x18            //FR: Flag register
#define UART_IBRD               0x24            //IBRD: Integer Baude Rate Divisor 
#define UART_FBRD               0x28            //FBRD: Fractional Baude Rate Divisor
#define UART_LCRH               0x2c            //LCRH: Line Control Register
#define UART_CR                 0x30            //CR: Control Register
#define WORD_SIZE               4
#define DEBUG                   0

void reg_setvalue(volatile uint32_t * addr, uint32_t value, uint32_t bits = 0, uint32_t shift = 0, uint32_t password = 0);

int main(int argc, char **argv)
{
    volatile uint32_t* virt_UART_IBRD, virt_UART_FBRD, virt_UART_LCRH, virt_UART_CR, virt_UART_FR;
    volatile uint32_t* virt_gpio, virt_uart;
    volatile uint32_t* map_base, *virt_base;
    int fd, count = 2400;

    printf("Peripherals base (word): %x\n", (uint32_t)BCM_2835_PERIPH_BASE);
    printf("GPIO base: %x\n", GPIO_OFFSET);
    printf("PWM base: %x\n", PWM_OFFSET);
    printf("CLK base: %x\n", CM_OFFSET);

    fd = open("/dev/mem", O_RDWR | O_SYNC, 0);

    map_base = (uint32_t *)mmap(0, BCM_2835_PERIPH_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BCM_2835_PERIPH_BASE);

    //Defining pointers to work in memory
    //Eksempel:
    virt_gpio = map_base + GPIO_OFFSET/WORD_SIZE;
    virt_sel1 = virt_gpio + GPFSEL1_addr/WORD_SIZE;

    virt_uart = map_base + UART_OFFSET/WORD_SIZE;
    virt_UART_IBRD = virt_uart + UART_IBRD/WORD_SIZE;
    virt_UART_FBRD = virt_uart + UART_FBRD/WORD_SIZE;
    virt_UART_LCRH = virt_uart + UART_LCRH/WORD_SIZE;
    virt_UART_CR = virt_uart + virt_UART_CR/WORD_SIZE;
    virt_UART_FR = virt_uart + virt_UART_FR/WORD_SIZE;

    //*** INITIALISERING ***

    //Set bit 29-27 to 010 (sets GPIO14 & GPIO15 to ALT0: RX & TX), BCM2835 pp. 92 & 102
    reg_setvalue(virt_gpio_sel1, 0b100, 3, 15);       //GPIO15
    reg_setvalue(virt_gpio_sel1, 0b100, 3, 12);       //GPIO14

    // __sync_synchronize();

    //TODO: DETERMINE FUARTCLK 
    //at the moment we have assumed it to be 400MHz

    //Set Baud-rate: 9600 bit/s 
    //BAUDDIV = (FUARTCLK/(16*Baud-rate)) --> BAUDDIV = 2604
    reg_setvalue(virt_UART_IBRD, 2604, 16);       //Integer part of baud divisor
    //reg_setvalue(virt_UART_FBRD, 0, 6);       //Fractional part of baud divisor

    //UART Line Control Register
    //bit1:     0b0 (No parity)
    //bit4:     0b0 (FIFO)
    //bit5-6:   0b11 (Word length 8 bit)
    reg_setvalue(virt_UART_LCRH, 0b0 | 0b1 <<4 | 0b11 << 8, 16);       

    //UART Control Register
    //Se procedure BCM2835 p. 185
    reg_setvalue(virt_UART_CR, 0b0, 1);              //Disable UART
    //Maybee insert a delay to ensure UART ready
    reg_setvalue(virt_UART_LCRH, 0b0 << 4);          //Disable FIFO
    //bit8:     0b1 (TX Enable)
    //bit9:     0b1 (RX Enable)
    reg_setvalue(virt_UART_CR, 0b1 << 8 | 0b1 << 9);
    reg_setvalue(virt_UART_CR, 0b0, 1);              //Enable UART
    reg_setvalue(virt_UART_LCRH, 0b1 << 4);          //Enable FIFO

    char *msg = "a";

    while(1) 
    {
        while( (*virt_UART_FR) >> 7 == 1)       //Wait for transmitter register empty
        {
            *virt_UART_DR = *msg;               //Put msg in DATA, automatically pushed to FIFO
        }
    }

    munmap(&map_base, BCM_2835_PERIPH_SIZE);

    return 0;
}

void reg_setvalue(volatile uint32_t * addr, uint32_t value, uint32_t bits, uint32_t shift, uint32_t password)
{

	for (int i = 0; i < bits; i++) {
		*addr = password | (*addr & ~(1 << (i + shift)) );
	}

	*addr = password | (*addr | (value << shift));
}
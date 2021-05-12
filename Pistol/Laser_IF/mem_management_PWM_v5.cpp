//Compile m. linjen:
//arm-rpizw-gcc -o mem_management_pwm mem_management_PWM_v3.c
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
#define CM_OFFSET               0x101000
#define GPIO_OFFSET             0x200000
#define PWM_OFFSET              0x20c000
#define PWM_CTRL_OFFSET         0x0
#define PWM_RANGE_OFFSET        0x20
#define PWM_DATA_OFFSET         0x24
#define CM_PWM_CTL_OFFSET       0x1010a0         //0x7e1010a0
#define CM_PWM_DIV_OFFSET       0x1010a4         //0x7e1010a4
#define BCM_2835_PASSWORD       (0x5A << 24)     //Password needed to set PWM clock
/* BCM2835 p. 91 ff. */
#define GPFSEL1_addr            0x04             //GPFSEL: define operation of the general-purpose I/O, GPFSEL1 Address offset for GPIO 10-19, 32-bit 
#define GPFSET0_addr            0x1C             //GPFSET: set GPIO pin (0: does nothing, 1: sets pin)
#define GPFCLR0_addr            0x28             //GPFCLR: clear GPIO pin (0: does nothing, 1: clears pin)
#define WORD_SIZE               4
#define DEBUG                   0

void reg_setvalue(volatile uint32_t * addr, uint32_t value, uint32_t bits, uint32_t shift = 0, uint32_t password = 0);

int main(int argc, char **argv)
{
    volatile uint32_t* virt_cm, *virt_gpio, *virt_pwm, *virt_gpio_sel1, *virt_pwm_ctl, *virt_pwm_range, *virt_pwm_data, *virt_cm_pwm_div, *virt_cm_pwm_ctl, *virt_gpio_set0, *virt_gpio_clr0;
    volatile uint32_t* map_base, *virt_base;
    int fd, count = 2400;

    printf("Peripherals base (word): %x\n", (uint32_t)BCM_2835_PERIPH_BASE);
    printf("GPIO base: %x\n", GPIO_OFFSET);
    printf("PWM base: %x\n", PWM_OFFSET);
    printf("CLK base: %x\n", CM_OFFSET);

    fd = open("/dev/mem", O_RDWR | O_SYNC, 0);

    map_base = (uint32_t *)mmap(0, BCM_2835_PERIPH_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BCM_2835_PERIPH_BASE);

    //Defining pointers to work in memory
    {
        virt_gpio = map_base + GPIO_OFFSET/WORD_SIZE;
        virt_pwm = map_base + PWM_OFFSET/WORD_SIZE;
        virt_cm_pwm_div = map_base + CM_PWM_DIV_OFFSET/WORD_SIZE;
        virt_cm_pwm_ctl = map_base + CM_PWM_CTL_OFFSET/WORD_SIZE;

        //Pointers to work on GPIO19
        virt_gpio_sel1 = virt_gpio + GPFSEL1_addr/WORD_SIZE;    //set pin operation 
        virt_pwm_ctl = virt_pwm + PWM_CTRL_OFFSET/WORD_SIZE;
        virt_pwm_range = virt_pwm + PWM_RANGE_OFFSET/WORD_SIZE;
        virt_pwm_data = virt_pwm + PWM_DATA_OFFSET/WORD_SIZE;
    }

    __sync_synchronize();

    //Set bit 29-27 to 010 (sets GPIO19 to ALT5: PWM1), BCM2835 pp. 92 & 102
    
    reg_setvalue(virt_gpio_sel1, 0b010, 3, 27);

    __sync_synchronize();

    /* Set clock divider to 16 (CM_PWM_DIV) */   
    //clm_pwm_div Integer part: 16 (0 0000 0001 0000), bit 23-12
    //clm_pwm_div Fractional part: 0, bit 11-0
    reg_setvalue(virt_cm_pwm_div, 1 << 16, 24, 0, BCM_2835_PASSWORD);

    __sync_synchronize();

    /* Set clock controls (CM_PWM_CTL) */
    //clm_pwm_ctl Enable: 1, bit 4
    //clm_pwm_ctl Source: 0x1, bit 3-0
    reg_setvalue(virt_cm_pwm_ctl, 1 << 4 | 1 << 0, 5, 0, BCM_2835_PASSWORD);

    __sync_synchronize();

    //Sæt PWM control (CTL) til at enable mark-space (MSEN2) og enable channel 2 (PWEN2) (p. 142)    
    reg_setvalue(virt_pwm_ctl, 1 << 15 | 1 << 8, 0);

    __sync_synchronize();

    //Sæt PWM range 2, max 32-bit (p. 147)    
    reg_setvalue(virt_pwm_range, count, 32);

    __sync_synchronize();

    //Sæt PWM data2 
    reg_setvalue(virt_pwm_data, count/2, 32);

    __sync_synchronize();

    printf("GPIO-BASE: %p\n", virt_gpio);

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

//printf("Value: %x at %p\n", *addr, addr);
//printf("bcm2835_peri_write_nb paddr %08X, value %08X\n", (unsigned) paddr, value);
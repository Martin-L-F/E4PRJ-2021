#include "Laser_IF.hpp"

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

void reg_setvalue(volatile uint32_t * addr, uint32_t value, uint32_t clearBits, uint32_t shift = 0, uint32_t password = 0);

Laser_IF::Laser_IF(int freq)
{
    setFrequency(freq);

    volatile uint32_t* map_base;
    int fd;

    fd = open("/dev/mem", O_RDWR | O_SYNC, 0);

    map_base = (uint32_t *)mmap(0, BCM_2835_PERIPH_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BCM_2835_PERIPH_BASE);

    //Defining pointers to work in memory
    virt_gpio = map_base + GPIO_OFFSET/WORD_SIZE;
    virt_pwm = map_base + PWM_OFFSET/WORD_SIZE;
    virt_cm_pwm_div = map_base + CM_PWM_DIV_OFFSET/WORD_SIZE;
    virt_cm_pwm_ctl = map_base + CM_PWM_CTL_OFFSET/WORD_SIZE;
    virt_gpio_sel1 = virt_gpio + GPFSEL1_addr/WORD_SIZE;    //set pin operation 
    virt_pwm_ctl = virt_pwm + PWM_CTRL_OFFSET/WORD_SIZE;
    virt_pwm_range = virt_pwm + PWM_RANGE_OFFSET/WORD_SIZE;
    virt_pwm_data = virt_pwm + PWM_DATA_OFFSET/WORD_SIZE;

    __sync_synchronize();

    //Set bit 29-27 to 010 (sets GPIO19 to ALT5: PWM1), BCM2835 pp. 92 & 102
    reg_setvalue(virt_gpio_sel1, 0b010, 3, 27);

    __sync_synchronize();

    /* Set clock divider to 16 (CM_PWM_DIV) */   
    //Integer part:     16, bit 23-12
    //Fractional part:  0, bit 11-0
    reg_setvalue(virt_cm_pwm_div, 1 << 16, 24, 0, BCM_2835_PASSWORD);

    __sync_synchronize();

    /* Set clock controls (CM_PWM_CTL) */
    //clm_pwm_ctl Enable: 1, bit 4
    //clm_pwm_ctl Source: 1, bit 3-0
    reg_setvalue(virt_cm_pwm_ctl, 1 << 4 | 1 << 0, 5, 0, BCM_2835_PASSWORD);

    __sync_synchronize();

    //Sæt PWM control (CTL) til at enable mark-space (MSEN2) og enable channel 2 (PWEN2) (p. 142)    
    reg_setvalue(virt_pwm_ctl, 1 << 15 | 1 << 8, 0);

    __sync_synchronize();

        //Sæt PWM range 2, max 32-bit (p. 147)    
    reg_setvalue(virt_pwm_range, PWMcounter_, 32);

    __sync_synchronize();

    //Sæt PWM data2 = 0 (OCRn)
    reg_setvalue(virt_pwm_data, 0, 32);

    __sync_synchronize();

    munmap(&map_base, BCM_2835_PERIPH_SIZE);
}

void Laser_IF::setFrequency(int freq)
{
    if( 0 < freq && freq <= 2000){
        frequency_ = freq;
        setPWMcounter();
    }
    else {
        frequency_ = 500;   //default value
        setPWMcounter();
        printf("ERROR: PWM frequency out of range, range 1-2000Hz\n");
    }
}

void Laser_IF::setPWMcounter()
{
    //clk_freq = 19.2MHz, clk_div = 16;
    //current_clk_freq = clk_freq/clk_div
    int current_clk_freq = 1200000;     

    PWMcounter_ = current_clk_freq/frequency_;
    //printf("PWMcounter_ = %i\n", PWMcounter_);
}

void Laser_IF::shootLaser()
{
    volatile uint32_t* map_base;
    int fd;

    fd = open("/dev/mem", O_RDWR | O_SYNC, 0);

    map_base = (uint32_t *)mmap(0, BCM_2835_PERIPH_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BCM_2835_PERIPH_BASE);

    __sync_synchronize();

    //Sæt PWM range 2, max 32-bit (p. 147)    
    reg_setvalue(virt_pwm_range, PWMcounter_, 32);

    __sync_synchronize();

    //Sæt PWM data2 (OCRn)
    reg_setvalue(virt_pwm_data, PWMcounter_ / 2, 32);

    usleep(shotLength_);

    //Sæt PWM data2 = 0 (OCRn)
    reg_setvalue(virt_pwm_data, 0, 32);

    __sync_synchronize();

    munmap(&map_base, BCM_2835_PERIPH_SIZE);
}

void reg_setvalue(volatile uint32_t * addr, uint32_t value, uint32_t clearBits, uint32_t shift, uint32_t password)
{

	for (int i = 0; i < clearBits; i++) {
		*addr = password | (*addr & ~(1 << (i + shift)) );
	}

	*addr = password | (*addr | (value << shift));
}
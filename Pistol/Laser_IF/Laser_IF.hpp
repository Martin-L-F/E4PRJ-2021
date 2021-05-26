#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <cstdint>

using namespace std;

class Laser_IF
{
public:
    Laser_IF(int freq = 500);   //500Hz default value
    void setFrequency(int freq);
    void shootLaser();
private:
    void setPWMcounter();
    int frequency_;             //valid values: 1-2000
    int PWMcounter_;
    int shotLength_ = 200000;   //length of shot in usek

    //Attributes used to map in memory
    volatile uint32_t* virt_cm, *virt_gpio, *virt_pwm, *virt_gpio_sel1, *virt_pwm_ctl, *virt_pwm_range, *virt_pwm_data, *virt_cm_pwm_div, *virt_cm_pwm_ctl, *virt_gpio_set0, *virt_gpio_clr0;
};
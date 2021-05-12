#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

class Laser_IF
{
public:
    Laser_IF();
    void setFrequency(int freq);
    void shootLaser();
private:
    void setPWMcounter();
    int _frequency;       //valid values: 0-2000
    int _PWMcounter;
    int _shotLength;      //length of shot in usek
};
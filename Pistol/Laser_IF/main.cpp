#include "Laser_IF.hpp"

int main(int argc, char **argv)
{
    int userInput = atoi(argv[1]);

    printf("%s\n", argv[1]);

    Laser_IF laserObj;

    laserObj.setFrequency(userInput);

    laserObj.shootLaser();

    return 0;
}
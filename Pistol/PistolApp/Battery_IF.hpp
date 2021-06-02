
#include <wiringPi.h>
#include <iostream>
#include <fstream>
#include <string>

class Battery_IF
{
public:
    Battery_IF()
    {
        wiringPiSetupGpio();
        pullUpDnControl(5, PUD_DOWN);
    }
    int getBatteryLevel()
    {
        std::ifstream ADC;
        ADC.open("/dev/LTC1096-0", std::ios::in);
        if (ADC.is_open())
        {
            int val;
            ADC >> val;
            if (val >= 0 && val < 6000)
            {
                storedVal = (val - 3000) / 20;
            }
            else
            {
                std::cout << "[Battery_IF][WARN] Input out of range. Input: '" << val << "'." << std::endl;
            }
            return storedVal;
        }
        else
        {
            std::cout << "[Battery_IF][ERROR] Could not open device." << std::endl;
            return storedVal;
        }
    }
    bool getCharging()
    {
        return digitalRead(5);
    }

private:
    int storedVal = 0;
};
#include <wiringPi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>

class Trigger_IF
{
public:
    Trigger_IF();
    ~Trigger_IF();
    void addFireCallback(std::function<void()> callback)
    {
        if (fireCallback != nullptr)
        {
            std::cout << "[Trigger][WARN] Overriding callback for: Fire" << std::endl;
        }
        fireCallback = callback;
    }

    void addEndOfFireCallback(std::function<void()> callback)
    {
        if (endOfFireCallback != nullptr)
        {
            std::cout << "[Trigger][WARN] Overriding callback for: EndOfFire" << std::endl;
        }
        endOfFireCallback = callback;
    }

private:
    void trigger(void);

    std::thread interuptThread;
    std::function<void()> fireCallback = nullptr;
    std::function<void()> endOfFireCallback = nullptr;
};

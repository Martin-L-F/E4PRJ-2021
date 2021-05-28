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
        if (_fireCallback != nullptr)
        {
            std::cout << "[Trigger][WARN] Overriding callback for: Fire" << std::endl;
        }
        _fireCallback = callback;
    }

private:
    void trigger(void);

    std::thread _interuptThread;
    std::function<void()> _fireCallback = nullptr;
};

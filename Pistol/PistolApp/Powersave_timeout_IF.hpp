#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>

class Powersave_timeout_IF
{
public:
    Powersave_timeout_IF();
    ~Powersave_timeout_IF();
    void addCallback(std::function<void()> callback)
    {
        if (_callback != nullptr)
        {
            std::cout << "[Powersave_timeout][WARN] Overriding callback for: Fire" << std::endl;
        }
        _callback = callback;
    }
    bool reset();
    bool stop();
    bool setDelay(int jiffies);

private:
    void trigger(void);

    std::thread _interuptThread;
    std::function<void()> _callback = nullptr;
};

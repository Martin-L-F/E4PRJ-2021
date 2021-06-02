#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>

class Magasin_IF
{
public:
    Magasin_IF();

    ~Magasin_IF();

    void addMagSwitchCallback(std::function<void()> callback)
    {
        if (_magSwitch != nullptr)
        {
            std::cout << "[Magasin][WARN] Overriding callback for: MagSwitch" << std::endl;
        }
        _magSwitch = callback;
    }

    bool decrementMag();

    void setMag(int difficulty);

    void fillMag();

private:
    void trigger(void);

    std::thread _interuptThread;
    std::function<void()> _magSwitch = nullptr;
    int _ammoAmount = 0;
    int _magSize = 0;
};

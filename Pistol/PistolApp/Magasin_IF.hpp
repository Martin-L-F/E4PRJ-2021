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
        if (magSwitch != nullptr)
        {
            std::cout << "[Magasin][WARN] Overriding callback for: MagSwitch" << std::endl;
        }
        magSwitch = callback;
    }

    bool decrementMag();

    void setMag(int difficulty);

private:
    void trigger(void);

    void fillMag();

    std::thread interuptThread;
    std::function<void()> magSwitch = nullptr;
    int ammoAmount = 0;
    int magSize = 0;
};

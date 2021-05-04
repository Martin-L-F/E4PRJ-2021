#include "Powersave_timeout_IF.hpp"

Powersave_timeout_IF::Powersave_timeout_IF()
{
    _interuptThread = std::thread(&Powersave_timeout_IF::trigger, this);
    reset();
    setDelay(6000);
}

Powersave_timeout_IF::~Powersave_timeout_IF()
{
    _interuptThread.join();
}

inline void Powersave_timeout_IF::trigger(void)
{
    while (true)
    {
        std::cout << "[Powersave_timeout][INFO] Starting interrupt thread" << std::endl;
        char *memblock = new char[1];
        std::ifstream powersave_timeout_Interupt;
        powersave_timeout_Interupt.open("/dev/targetbeam_powersave_timeout", std::ios::in);

        while (powersave_timeout_Interupt.is_open())
        {
            memblock = new char[1];
            powersave_timeout_Interupt.read(memblock, 1);

            if (_callback != nullptr)
            {
                _callback();
            }
            else
            {
                std::cout << "[Powersave_timeout][WARN] No callback" << std::endl;
            }
        }
        std::cout << "[Powersave_timeout][WARN] Could not open filestream. Restarting service in 5 seconds" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

bool Powersave_timeout_IF::reset()
{
    std::ofstream powersave_timeout_Interupt;
    powersave_timeout_Interupt.open("/dev/targetbeam_powersave_timeout", std::ios::out);
    if (!powersave_timeout_Interupt.is_open())
    {
        std::cout << "[Powersave_timeout][WARN] Reset failed. Could not open filestream" << std::endl;
        return false;
    }
    powersave_timeout_Interupt.write("r", 1);
    std::cout << "[Powersave_timeout][INFO] Reset complete" << std::endl;
    return true;
}
bool Powersave_timeout_IF::stop()
{
    std::ofstream powersave_timeout_Interupt;
    powersave_timeout_Interupt.open("/dev/targetbeam_powersave_timeout", std::ios::out);
    if (!powersave_timeout_Interupt.is_open())
    {
        std::cout << "[Powersave_timeout][WARN] Stop failed. Could not open filestream" << std::endl;
        return false;
    }
    powersave_timeout_Interupt.write("S", 1);
    std::cout << "[Powersave_timeout][INFO] Stop complete" << std::endl;
    return true;
}
bool Powersave_timeout_IF::setDelay(int jiffies)
{
    std::cout << "[Powersave_timeout][INFO] Initiating delay configuration" << std::endl;
    std::ofstream powersave_timeout_Interupt;
    powersave_timeout_Interupt.open("/dev/targetbeam_powersave_timeout", std::ios::out);
    if (!powersave_timeout_Interupt.is_open())
    {
        std::cout << "[Powersave_timeout][WARN] Set delay failed. Could not open filestream" << std::endl;
        return false;
    }
    std::string command = "I";
    command += std::to_string(jiffies);

    std::cout << "[Powersave_timeout][INFO] Adjusting delay to: '" << command << "'" << std::endl;

    powersave_timeout_Interupt.write(command.c_str(), command.length());
    return true;
}
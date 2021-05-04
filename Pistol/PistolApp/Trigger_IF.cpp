#include "Trigger_IF.hpp"

Trigger_IF::Trigger_IF()
{
    wiringPiSetupGpio();
    pullUpDnControl(21, PUD_DOWN);
    _interuptThread = std::thread(&Trigger_IF::trigger, this);
}

Trigger_IF::~Trigger_IF()
{
    _interuptThread.join();
}

inline void Trigger_IF::trigger(void)
{
    while (true)
    {
        std::cout << "[Trigger][INFO] Starting interrupt thread" << std::endl;
        char *memblock = new char[1];
        std::ifstream triggerInterupt;
        triggerInterupt.open("/dev/targetbeam_trigger", std::ios::in);

        while (triggerInterupt.is_open())
        {
            memblock = new char[1];
            while (memblock[0] == NULL)
            {
                triggerInterupt.read(memblock, 1);
            }

            do
            {
                if (_fireCallback != nullptr)
                {
                    _fireCallback();
                }
                else
                {
                    std::cout << "[Trigger][WARN] No callback for: Fire" << std::endl;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(500));

                if (_endOfFireCallback != nullptr)
                {
                    _endOfFireCallback();
                }
                else
                {
                    std::cout << "[Trigger][WARN] No callback for: EndOfFire" << std::endl;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            } while (digitalRead(21) == 1);
        }
        std::cout << "[Trigger][WARN] Could not open filestream. Restarting service in 5 seconds" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

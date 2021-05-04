#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>

#include "Trigger_IF.hpp"
#include "Magasin_IF.hpp"
#include "Powersave_timeout_IF.hpp"

class Pistol_Controller
{
public:
    Pistol_Controller()
    {
        _trigger.addFireCallback((std::function<void()>)std::bind(&Pistol_Controller::startShot, this));
        _trigger.addEndOfFireCallback((std::function<void()>)std::bind(&Pistol_Controller::endShot, this));

        _magasin.addMagSwitchCallback((std::function<void()>)std::bind(&Pistol_Controller::magSwitched, this));
        _magasin.setMag(1);

        _powersave_timeout.addCallback((std::function<void()>)std::bind(&Pistol_Controller::goToPowersave, this));
        _powersave_timeout.setDelay(50000);
    }

private:
    void startShot()
    {
        std::cout << "[CTLR][INFO] Fire" << std::endl;
        bool res = _magasin.decrementMag();
        //Reset timer
        //Fire laser
        //Play sound
    }

    void endShot()
    {
        std::cout << "[CTLR][INFO] End of fire" << std::endl;
    }

    void magSwitched()
    {
        std::cout << "[CTLR][INFO] Magasin switched" << std::endl;
        //Play sound
    }

    void goToPowersave()
    {
        std::cout << "[CTLR][INFO] Initiating powersave" << std::endl;
        // Disable Wi-Fi and power down

        _powersave_timeout.reset();
    }

    Trigger_IF _trigger;
    Magasin_IF _magasin;
    Powersave_timeout_IF _powersave_timeout;
};
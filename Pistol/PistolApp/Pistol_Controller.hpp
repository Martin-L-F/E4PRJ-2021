#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>

#include "Trigger_IF.hpp"
#include "Magasin_IF.hpp"

class Pistol_Controller
{
public:
    Pistol_Controller()
    {
        trigger.addFireCallback((std::function<void()>)std::bind(&Pistol_Controller::startShot, this));
        trigger.addEndOfFireCallback((std::function<void()>)std::bind(&Pistol_Controller::endShot, this));
        magasin.addMagSwitchCallback((std::function<void()>)std::bind(&Pistol_Controller::magSwitched, this));
        magasin.setMag(1);
    }

private:
    void startShot()
    {
        std::cout << "[CTLR][INFO] Fire" << std::endl;
        bool res = magasin.decrementMag();
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

    Trigger_IF trigger;
    Magasin_IF magasin;
};
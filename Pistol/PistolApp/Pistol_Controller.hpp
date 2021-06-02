#include "Laser_IF.hpp"
#include "Rumble_IF.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>

#include "Trigger_IF.hpp"
#include "Magasin_IF.hpp"
#include "Powersave_timeout_IF.hpp"

#include "WebSocketClient_IF.hpp"
#include "Battery_IF.hpp"
#include "Speaker_IF.h"

class Pistol_Controller
{
public:
    Pistol_Controller()
    {
        readConfigfile();
        trigger.addFireCallback((std::function<void()>)std::bind(&Pistol_Controller::startShot, this));

        magasin.addMagSwitchCallback((std::function<void()>)std::bind(&Pistol_Controller::magSwitched, this));
        magasin.setMag(1);

        powersave_timeout.addCallback((std::function<void()>)std::bind(&Pistol_Controller::goToPowersave, this));
        powersave_timeout.setDelay(50000);

        client.set_onMessage_handler(std::bind(&Pistol_Controller::onMessage, this, std::placeholders::_1));
        client.set_onConnected_handler(std::bind(&Pistol_Controller::onConnected, this));
        client.set_getBattery_function(std::bind(&Pistol_Controller::getBatteryLevel, this));
        client.set_getCharging_function(std::bind(&Pistol_Controller::getCharging, this));
    }
    void run()
    {
        std::cout << std::endl
                  << " ============================ " << std::endl
                  << " === TargetBeam PistolApp === " << std::endl
                  << " ============================ " << std::endl
                  << std::endl;
        client.run();
    }

private:
    void onMessage(json message)
    {
        if (message["type"] == "armPistol")
        {

            if (!message["difficulty"].is_string() || !message["frequency"]["range"].is_number() || !message["frequency"]["index"].is_number())
            {
                std::cout << "[Pistol_Controller][WARN] Content error in message of type: 'armPistol'." << std::endl;
                return;
            }
            if (message["difficulty"] == "easy")
            {
                difficulty = 1;
            }
            else if (message["difficulty"] == "medium")
            {
                difficulty = 2;
            }
            else if (message["difficulty"] == "hard")
            {
                difficulty = 3;
            }
            int index = message["frequency"]["index"];
            laser.setFrequency((index * frequency_a) + frequency_b);

            GameActive = true;
        }

        else if (message["type"] == "preventStandby")
        {
            powersave_timeout.stop();
        }

        else if (message["type"] == "disarmPistol")
        {
            GameActive = false;
            difficulty = 0;
            powersave_timeout.reset();
        }

        else
        {
            std::cout << "[Pistol_Controller][WARN] Unresolved message type" << std::endl;
        }
    }

    void onConnected()
    {
        speaker.playSound("Sounds/success");
    }

    void startShot()
    {
        std::cout << "[CTLR][INFO] Fire" << std::endl;
        if (!GameActive)
        {
            speaker.playSound("Sounds/failure");
            powersave_timeout.reset();
            return;
        }
        bool res = magasin.decrementMag();
        if (res)
        {
            speaker.playSound("Sounds/shot");
            rumble.startRumble();
            laser.shootLaser();
        }
        else
        {
            speaker.playSound("Sounds/noammo");
        }
    }

    void magSwitched()
    {
        std::cout << "[CTLR][INFO] Magasin switched" << std::endl;
        magasin.fillMag();
        speaker.playSound("Sounds/reloaded");
    }

    void goToPowersave()
    {
        std::cout << "[CTLR][INFO] Initiating powersave" << std::endl;
        // Disable Wi-Fi and power down
        speaker.playSound("Sounds/standby");
    }

    int getBatteryLevel()
    {
        return battery.getBatteryLevel();
    }

    bool getCharging()
    {
        return battery.getCharging();
    }
    void readConfigfile()
    {
        try
        {
            std::ifstream configStream("config.json");
            json config;
            if (configStream.is_open())
            {
                std::cout << "[Pistol_Controller][INFO] Configurationfile found." << std::endl;
                configStream >> config;
                configStream.close();

                if (config.is_object())
                {
                    // Get frequency_b
                    if (config["Pistol_Controller"]["frequency_b"].is_number())
                    {
                        frequency_b = config["Pistol_Controller"]["frequency_b"];
                        std::cout << "[Pistol_Controller][INFO] Frequency_b changed to '" << frequency_b << "'." << std::endl;
                    }

                    // Get frequency_a
                    if (config["Pistol_Controller"]["frequency_a"].is_number())
                    {
                        frequency_a = config["Pistol_Controller"]["frequency_a"];
                        std::cout << "[Pistol_Controller][INFO] Frequency_a changed to '" << frequency_a << "'." << std::endl;
                    }
                }
            }
        }
        catch (const std::exception &ex)
        {
            std::cout << "[Pistol_Controller][WARN] An error occurred while reading configurationfile." << std::endl;
            std::cout << "[Pistol_Controller][WARN] " << ex.what() << std::endl;
        }
    }

    Trigger_IF trigger;
    Magasin_IF magasin;
    Powersave_timeout_IF powersave_timeout;
    WebSocketClient_IF client;
    Battery_IF battery;
    Speaker_IF speaker;
    Laser_IF laser;
    Rumble_IF rumble;
    int difficulty = 0;
    bool GameActive = false;
    int frequency_b = 500;
    int frequency_a = 100;
};
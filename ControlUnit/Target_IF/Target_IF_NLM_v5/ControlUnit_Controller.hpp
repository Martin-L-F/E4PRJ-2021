#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>
#include <functional>

#include "UserData_Storage.hpp"
#include "Target_IF.hpp"
#include "UART/MsgQueue.hpp"
#include "RailSystem_IF.hpp"

using namespace std;
using namespace chrono;

//Purely made for Target_IF and RailSstem test. Just change it.
class ControlUnit_Controller
{
public:
    ControlUnit_Controller()
    {
        TargetObj.addCallback(bind(&ControlUnit_Controller::shotDetected, this, placeholders::_1));
        RailSystemObj.addCallback(bind(&ControlUnit_Controller::CalibrateComplete, this));

    }
    Target_IF TargetObj;
    RailSystem_IF RailSystemObj;
private:
    UserData_Storage UserDataStorage_;
    void CalibrateComplete()
    {
        cout << "Calibrate complete" << endl;
        usleep(1000);
        RailSystemObj.startMoving(1);
        cout << "Start moving" << endl;

        sleep(3);

        RailSystemObj.stopMoving();
        cout << "Stop moving" << endl;
    }
    void shotDetected(int player)
    {
        UserDataStorage_.UpdatePlayerScore(player);
    }

};
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>
#include <functional>

#include "UserData_Storage.hpp"
#include "Target_IF.hpp"
#include "UART/MsgQueue.hpp"

using namespace std;
using namespace chrono;

class ControlUnit_Controller
{
public:
    ControlUnit_Controller()
    {
        TargetObj.addCallback(bind(&ControlUnit_Controller::shotDetected, this, placeholders::_1));


    }

    void shotDetected(int player) {
        UserDataStorage_.UpdatePlayerScore(player);
    }
    MsgQueue UART_msgQueueObj;
    Target_IF TargetObj;
private:
    UserData_Storage UserDataStorage_;

};
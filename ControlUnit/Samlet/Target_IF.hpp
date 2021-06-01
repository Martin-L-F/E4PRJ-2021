#pragma once
#include "UART/UART.hpp"
#include "UART/Message.hpp"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <functional>

#include "json.hpp"
using json = nlohmann::json;

using namespace std;
using namespace chrono;

#define BUF_SIZE 32
#define Target_IF_PSOC_ID "0" //"0C0E040A00228400" //PSOC USB-ID number as string: KitProg_"this number"-if02. Put "0" if there is none

class Target_IF
{
public:
    Target_IF();
    void startDetection(int numberOfPlayers);
    void stopDetection();
    void UARTDispatcher(unsigned id, Message *msg);
    int updateScore(Message *user);
    void addCallback(function<void(int)> cb);

private:
    void readConfigfile();
    UART UARTobj_;
    string device;
    function<void(int)> callbackControllershotDetected_;
};

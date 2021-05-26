// #pragma once
// #include "UART/UART.hpp"
// #include "UART/Message.hpp"
// #include <stdio.h>

// #define BUF_SIZE 32
// #define RailSystem_IF_PSOC_ID "0C0E040A00228400" //PSOC USB-ID number as string


// class RailSystem_IF
// {
// public:
//     RailSystem_IF();
//     ~RailSystem_IF();
//     void startDetection(int numberOfPlayers);
//     void stopDetection();
// private:
//     UART UARTobj_;
// };


// int updateScore(Message* user); //Handler in TargetIF_Dispatcher
// void TargetIF_Dispatcher(unsigned id, Message* msg);
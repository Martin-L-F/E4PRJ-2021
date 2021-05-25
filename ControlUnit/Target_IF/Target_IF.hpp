<<<<<<< HEAD
#include "UART_IF.hpp"

class Target_IF : UART_IF
{
public:
    void startDetection(int);
    void stopDetection();
    int updateScore(int);
private:
    UART_IF UARTobj_;
=======
#pragma once
#include "UART.hpp"
#include <stdio.h>
#define BUF_SIZE 32



class Target_IF
{
public:
    Target_IF();
    ~Target_IF();
    void startDetection(int numberOfPlayers);
    void stopDetection();
    int updateScore(int user);
private:
    UART UARTobj_;
>>>>>>> dc75ac34de747faa3793aa28dbceb760239f8254
};
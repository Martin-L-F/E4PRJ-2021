#include "UART_IF.hpp"

class Target_IF : UART_IF
{
public:
    void startDetection(int);
    void stopDetection();
    int updateScore(int);
private:
    UART_IF UARTobj_;
};
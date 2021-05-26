#pragma once
#include "UART/Message.hpp"
#include "UART/MsgQueue.hpp"

#include <cstdint>

struct Rx_data : public Message
{
    Rx_data(uint8_t* rx_mem) {
        for (size_t i = 0; i < 3; i++)
        {
            rx_mem_[i] = rx_mem[i];
        }
        
    }
    uint8_t rx_mem_[3];
};

/****Target_IF****/
enum{ DummyID, ShotDetected };

void *funcTargetIF_UARTReceive(void *arg);
void *funcTargetIF_Dispatcher(void* arg);

/****RailSystem_IF****/
enum{ DummyID2, CalibrateComplete };
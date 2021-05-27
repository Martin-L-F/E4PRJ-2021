#include "Rx_data.hpp"

Rx_data::Rx_data(uint8_t* rx_mem)
{
    for (int i = 0; i < 3; i++)
    {
        rx_mem_[i] = rx_mem[i];
    }
}

Rx_data::~Rx_data()
{
}

int Rx_data::getMsgData() const
{
    return rx_mem_[1];
}
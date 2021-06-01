#include <cstdint>

#include "Message.hpp"

class Rx_data : public Message
{
public:
    Rx_data(uint8_t*);
    ~Rx_data();
    int getMsgData() const;
private:
    uint8_t rx_mem_[3];
};

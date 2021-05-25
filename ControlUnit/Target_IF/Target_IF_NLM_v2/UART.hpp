#pragma once
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdint>

#define debug_UART 0

class UART
{
public:
    UART();
    ~UART();
    void writeMsg(uint8_t *, uint8_t);
    uint8_t readMsg(uint8_t *, uint8_t) const;
private:
    int fd_;
};
#pragma once
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

class UART
{
public:
    UART();
    ~UART();
    void writeMsg(char *);
    int readMsg(char *, int) const;
private:
    int fd_;
};
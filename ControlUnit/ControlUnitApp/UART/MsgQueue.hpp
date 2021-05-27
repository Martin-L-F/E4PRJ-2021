#pragma once
#include "Message.hpp"
#include "ScopedLocker.hpp"
#include <queue>
#include <stdlib.h>
#include <threads.h>

using namespace std;

class MsgQueue
{
public:
    MsgQueue(unsigned long maxSize = 1);
    void send(unsigned long id, Message *msg = NULL);
    Message* receive(unsigned long &id);
    ~MsgQueue();
private:

    unsigned long maxSize_;
    struct Item : Message
    {
        Item(unsigned long id , Message *msg) : id_(id), msg_(msg) {}
        unsigned long id_;
        Message* msg_;
    };
    queue<Item> MsgContainer_;
    
    pthread_mutex_t MQmux;
    pthread_cond_t s_receive;
    pthread_cond_t s_send;
};
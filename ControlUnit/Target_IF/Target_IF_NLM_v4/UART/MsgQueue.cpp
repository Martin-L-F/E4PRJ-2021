#include "MsgQueue.hpp"
#include <list>
#include "iostream"

MsgQueue::MsgQueue(unsigned long maxSize) : maxSize_(maxSize)
{
    int err;
    err = pthread_mutex_init(&MQmux, nullptr);
    if (err < 0)
        cout << "Mutex entry init error" << endl;

    pthread_cond_init(&s_send, nullptr);
    pthread_cond_init(&s_receive, nullptr);
}

void MsgQueue::send(unsigned long id, Message *msg)
{
    //cout << "MQ Send trying to lock mux" << endl;
    ScopedLocker SLobj(&MQmux);
    //cout << "MQ send locked mux" << endl;

    while(MsgContainer_.size() >= maxSize_) {
        //cout << "Send unlocks mux and waits for signal" << endl;
        //pthread_cond_signal(&s_receive);
        pthread_cond_wait(&s_send, &MQmux);
    }
    MsgContainer_.push(Item(id, msg));
    //cout << "Container size: " << MsgContainer_.size() << endl;

    pthread_cond_signal(&s_receive);


    //cout << "Message pushed and unlocking mux" << endl;
}

Message* MsgQueue::receive(unsigned long &id)
{
    //cout << "MQ Receive trying to lock mux" << endl;
    ScopedLocker SLobj(&MQmux);
    //cout << "MQ Receive locked mux" << endl;
    while(MsgContainer_.size() == 0) {
        //cout << "Receive unlocks mux and waits for signal" << endl;
        //pthread_cond_signal(&s_send);
        pthread_cond_wait(&s_receive, &MQmux);
    }
    //cout << "MQ Receive Got signal and mux while queue is active" << endl;

    id = MsgContainer_.front().id_;
    Message* tempMsg = MsgContainer_.front().msg_;
    MsgContainer_.pop();
    pthread_cond_signal(&s_send);

    //cout << "Pop og return message and unlock mux" << endl;
    return tempMsg;
}

MsgQueue::~MsgQueue()
{

}
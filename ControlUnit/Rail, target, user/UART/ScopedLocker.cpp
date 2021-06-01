#include "ScopedLocker.hpp"

ScopedLocker::ScopedLocker(pthread_mutex_t* muxObjptr) : muxObjptr_(muxObjptr)
{
    pthread_mutex_lock(muxObjptr_);
}

ScopedLocker::~ScopedLocker()
{
    pthread_mutex_unlock(muxObjptr_);
}
#ifndef SCOPED_LOCKER_HPP_
#define SCOPED_LOCKER_HPP_

#include <pthread.h>

class ScopedLocker
{
public:
   ScopedLocker(pthread_mutex_t* muxObjptr);
   ~ScopedLocker();
private:
   pthread_mutex_t* muxObjptr_;
};

#endif
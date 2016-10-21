#include "lock.h"

BMLock::BMLock()
{

}

BMLock::~BMLock()
{

}

void BMLock::TryLock(unsigned int ms)
{
    mtx_.tryLock(ms);
}

void BMLock::Lock(){
    mtx_.lock();
}

void BMLock::Unlock()
{
    mtx_.unlock();
}


BMLockFactory::BMLockFactory(){

}

ILock * BMLockFactory::CreateLock()
{
    return new BMLock();
}

void BMLockFactory::FreeLock(ILock *lock)
{
    delete (BMLock *)lock;
}

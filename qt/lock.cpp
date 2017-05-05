#include "lock.h"

BMLock::BMLock()
{

}

BMLock::~BMLock()
{

}

void BMLock::TryLock(unsigned int ms)
{
    //mtx_.tryLock(ms);
}

void BMLock::Lock(){
    //mtx_.lock();
}

void BMLock::Unlock()
{
    //mtx_.unlock();
}


BMLockCraft::BMLockCraft(){

}

BMLockCraft::~BMLockCraft()
{

}

ILock * BMLockCraft::CreateLock()
{
    return new BMLock();
}

void BMLockCraft::FreeLock(ILock *lock)
{
    delete (BMLock *)lock;
}

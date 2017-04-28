#ifndef LOCK_H
#define LOCK_H

#include <QMutex>

#include <bitmailcore/bitmail.h>

class BMLock : public ILock
{
public:
    BMLock();
    virtual ~BMLock();
public:
    virtual void Lock();
    virtual void TryLock(unsigned int ms);
    virtual void Unlock();
private:
    QMutex mtx_;
};

class BMLockCraft : public ILockCraft
{
public:
    BMLockCraft();
    virtual ~BMLockCraft();
public:
    virtual ILock * CreateLock();
    virtual void FreeLock(ILock * lock);
};

#endif // LOCK_H

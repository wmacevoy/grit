/*!
  \file
  \brief ���b�N�N���X

  \author Satofumi KAMIMURA

  $Id: Lock.cpp 772 2009-05-05 06:57:57Z satofumi $
*/

#include "Lock.h"
#include "SdlInit.h"
#include <SDL/SDL_thread.h>

using namespace qrk;


struct Lock::pImpl : private qrk::SdlInit
{
  friend class ConditionVariable;

  SDL_mutex* mutex_;


  pImpl(void) : mutex_(SDL_CreateMutex())
  {
  }


  ~pImpl(void)
  {
    SDL_DestroyMutex(mutex_);
  }
};


Lock::Lock(void) : pimpl(new pImpl)
{
}


Lock::~Lock(void)
{
}


void Lock::lock(void)
{
  SDL_LockMutex(pimpl->mutex_);
}


void Lock::unlock(void)
{
  SDL_UnlockMutex(pimpl->mutex_);
}


//////////////////////////////////////////////////////////////////////
/*!
  \file
  \brief �����ϐ�

  Lock::pImpl �̃����o�ϐ��𑀍삷�邽�߂ɁALock.cpp �Ŏ������Ă���

  \author Satofumi KAMIMURA

  $Id: Lock.cpp 772 2009-05-05 06:57:57Z satofumi $
*/


struct ConditionVariable::pImpl : private SdlInit
{
  SDL_cond* condition_;
  bool is_waiting_;


  pImpl(void) : condition_(SDL_CreateCond()), is_waiting_(false)
  {
  }


  ~pImpl(void) {
    SDL_DestroyCond(condition_);
  }
};


ConditionVariable::ConditionVariable(void) : pimpl(new pImpl)
{
}


ConditionVariable::~ConditionVariable(void)
{
}


bool ConditionVariable::isWaiting(void)
{
  return pimpl->is_waiting_;
}


void ConditionVariable::wakeup(void)
{
  SDL_CondSignal(pimpl->condition_);
  pimpl->is_waiting_ = false;
}


void ConditionVariable::wakeupAll(void)
{
  SDL_CondBroadcast(pimpl->condition_);
  pimpl->is_waiting_ = false;
}


bool ConditionVariable::wait(Lock* lock, int timeout)
{
  int ret = 0;
  pimpl->is_waiting_ = true;
  if (timeout == NoTimeout) {
    ret = SDL_CondWait(pimpl->condition_, lock->pimpl->mutex_);
  } else {
    ret = SDL_CondWaitTimeout(pimpl->condition_, lock->pimpl->mutex_, timeout);
  }
  if (ret == 0) {
    return true;
  } else {
    pimpl->is_waiting_ = false;
    return false;
  }
}

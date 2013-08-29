/*!
  \file
  \brief ���j�^�C�x���g�Ǘ�

  \author Satofumi KAMIMURA

  $Id: MonitorEventScheduler.cpp 1483 2009-11-01 23:58:16Z satofumi $
*/

#include "MonitorEventScheduler.h"
#include "../system/Thread.h"
#include "../system/Lock.h"
#include "../system/ticks.h"
#include "../system/delay.h"
#include <list>

using namespace qrk;
using namespace std;


struct MonitorEventScheduler::pImpl
{
  enum {
    ThreadMaxDelay = 200,       // [msec]
  };

  class event_t
  {
  public:
    int ticks;
    ConditionVariable* condition;

    event_t(int ticks_, ConditionVariable* condition_)
      : ticks(ticks_), condition(condition_)
    {
    }

    bool operator < (const event_t& rhs) const
    {
      return ticks < rhs.ticks;
    }
  };

  Lock mutex_;
  Thread thread_;
  list<event_t> event_list_;

  ConditionVariable wait_condition_;
  bool terminated_;


  pImpl(void) : thread_(schedule_thread, this), terminated_(false)
  {
    thread_.run(Thread::Infinity);
  }


  ~pImpl(void)
  {
    terminate();
  }


  static void eventDelay(int delay_msec)
  {
    if (delay_msec > ThreadMaxDelay) {
      // Thread �̎����ɂ���ẮA�X���b�h�͒��I�ɏI�����邱�Ƃ��]�܂���
      // ���̂��߂̑ҋ@��Ԃ̍ő�l
      delay_msec = ThreadMaxDelay;
    }
    delay((delay_msec > 0) ? delay_msec : 1);
  }


  static int schedule_thread(void* args)
  {
    pImpl* obj = static_cast<pImpl*>(args);
    obj->mutex_.lock();
    if (obj->terminated_) {
      obj->mutex_.unlock();
      return 0;
    }

    // ���݂� ticks ���擾���A��菬���� ticks �̃C�x���g�����ԂɋN����
    // !!! �ꎞ��~���l�����ׂ�
    int current_ticks = ticks();
    list<event_t>::iterator end_it = obj->event_list_.end();
    for (list<event_t>::iterator it = obj->event_list_.begin();
         it != end_it;) {
      if (current_ticks < it->ticks) {
        break;
      }

      if (! it->condition->isWaiting()) {
        // �҂���ԂłȂ���΁A���̎��ŏ�������
        // �ȉ��� delay() �ȍ~�ɁA�Ăяo�����ő҂���ԂɑJ�ڂ���͂�
        int delay_msec = it->ticks - current_ticks;
        obj->mutex_.unlock();
        eventDelay(delay_msec);
        return 0;
      }

      ConditionVariable* condition = it->condition;
      it = obj->event_list_.erase(it);
      condition->wakeup();
    }

    if (obj->event_list_.empty()) {
      // �҂���Ԃɓ��
      eventDelay(1);
      obj->wait_condition_.wait(&obj->mutex_);
      obj->mutex_.unlock();

    } else {
      // ���̃C�x���g�܂őҋ@������
      int delay_msec = obj->event_list_.front().ticks - current_ticks;
      obj->mutex_.unlock();
      eventDelay(delay_msec);
    }
    return 0;
  }


  void terminate(void)
  {
    if (! thread_.isRunning()) {
      return;
    }

    mutex_.lock();
    terminated_ = true;

    // �Ǘ����Ă���^�X�N��S�ċN��������
    list<event_t>::iterator end_it = event_list_.end();
    for (list<event_t>::iterator it = event_list_.begin();
         it != end_it;) {
      ConditionVariable* condition = it->condition;
      it = event_list_.erase(it);
      condition->wakeup();
    }
    mutex_.unlock();

    if (wait_condition_.isWaiting()) {
      wait_condition_.wakeup();
    }
    thread_.stop();
    thread_.wait();
  }
};


MonitorEventScheduler::MonitorEventScheduler(void) : pimpl(new pImpl)
{
}


MonitorEventScheduler::~MonitorEventScheduler(void)
{
}


MonitorEventScheduler* MonitorEventScheduler::object(void)
{
  static MonitorEventScheduler singleton_object;
  return &singleton_object;
}


void MonitorEventScheduler::terminate(void)
{
  pimpl->terminate();
}


void MonitorEventScheduler::registerWakeupTicks(ConditionVariable* condition,
                                                int ticks)
{
  pimpl->mutex_.lock();
  if (pimpl->terminated_) {
    pimpl->mutex_.unlock();
    return;
  }

  // �C�x���g���X�g�ɒǉ�
  pImpl::event_t event(ticks, condition);
  list<pImpl::event_t> add_event;
  add_event.push_back(event);
  pimpl->event_list_.merge(add_event);
  pimpl->mutex_.unlock();

  // �C�x���g����̎��̒ǉ��ŁA�X�P�W���[���̃X���b�h���N����
  if (pimpl->wait_condition_.isWaiting()) {
    pimpl->wait_condition_.wakeup();
  }
}


void MonitorEventScheduler::registerDeviceServer(DeviceServer* device)
{
  static_cast<void>(device);
  // !!!

  // !!! �o�^��A�ڑ����s�킹�邽�߂ɁA�X���b�h�𓮍삳����
  // !!!
}

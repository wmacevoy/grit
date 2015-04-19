#pragma once

#include <mutex>

class Lock
{
 public:
  std::mutex &m;
  inline Lock(std::mutex &m_) : m(m_) { m.lock(); }
  inline ~Lock() { m.unlock(); }
};

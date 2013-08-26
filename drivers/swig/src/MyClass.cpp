#include "MyClass.h"

void MyClass::value(int value_)
{
  m_value = value_;
}

int MyClass::value() const
{
  return m_value;
}

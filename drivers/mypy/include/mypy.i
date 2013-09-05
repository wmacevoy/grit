%module mypy

%{
class MyClass
{
  int m_value;
 public:
  float x;
  void value(int value_);
  int value() const;
};
%}

class MyClass
{
  int m_value;
 public:
  float x;
  void value(int value_);
  int value() const;
};

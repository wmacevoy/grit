#include "Servo.h"
#include <iostream>\

Servo::~Servo() 
{ 
	m_id = 0; 
	m_name = ""; 
}

void Servo::id(int id_) { m_id=id_; }
int Servo::id() const { return m_id; }

void Servo::name(const std::string &name_) { m_name=name_; }
const std::string &Servo::name() const { return m_name; }

void Servo::report(std::ostream &out) const
{
  out << name() << "," << id() << "," << angle();
}

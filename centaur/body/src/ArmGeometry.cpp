#include "ArmGeometry.h"

void ArmGeometry::name(const std::string &name)
{
  m_name=name;
}

const std::string &ArmGeometry::name() const
{
  return m_name;
}

ArmGeometry::~ArmGeometry() {}



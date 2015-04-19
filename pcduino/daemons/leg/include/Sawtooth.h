#pragma once

class Sawtooth
{
 public:
  double L;
  double w;

  Sawtooth(double L_, double w_);
  double operator()(double x);
};

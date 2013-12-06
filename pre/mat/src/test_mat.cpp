#include "mat.h"
#include <iostream>
#include <assert.h>
#include <math.h>

using namespace std;

void eq(const Vec &a, const Vec &b)
{
  for (size_t i=0; i<a.size(); ++i) {
    cout << "a[" << i << "]=" << a[i] << endl;
  }
  for (size_t i=0; i<a.size(); ++i) {
    cout << "b[" << i << "]=" << b[i] << endl;
  }
  for (size_t i=0; i<a.size(); ++i) {
    cout << "a[" << i << "]=" << a[i] << endl;
    assert(fabs(eval(a[i]-b[i]))<1e-6);
  }
}

ostream& operator<<(ostream &out, const Vec &v)
{
  out << "[";
  for (size_t i=0; i!=v.size(); ++i) {
    if (i > 0) out << ",";
    out << v[i];
  }
  out << "]";
  return out;
}

void rotations()
{
  // turn counter-clockwise as the axis faces me

  Mat Rx=rotate(o,ex,var("%pi")/num(2));
  Mat Ry=rotate(o,ey,var("%pi")/num(2));
  Mat Rz=rotate(o,ez,var("%pi")/num(2));

  eq(Rz*ex,ey);
  eq(Rx*ey,ez);
  eq(Ry*ez,ex);
}

int main()
{
  rotations();
  E pi=var("%pi");

  E tx=var("tx");
  E ty=var("ty");
  E tz=var("tz");
  Vec t=vec(tx,ty,tz);

  E theta=var("theta");

  cout << tx << endl;
  cout << tx+ty << endl;
  cout << tx-ty << endl;
  cout << tx*ty << endl;
  cout << tx/ty << endl;
  cout << pow(tx,ty) << endl;
  cout << cos(tx)<< endl;
  cout << sin(tx)<< endl;

  Mat T = translate(t);
  Mat R = rotate(o,ex,cos(theta),sin(theta));
  Mat RT = R*T;
  Mat TR = T*R;
  Mat BracketRT = R*T-T*R;

  define(cout,"T",T);
  define(cout,"R",R);
  define(cout,"RT",RT);
  define(cout,"TR",TR);
  define(cout,"[R,T]",BracketRT);
}

#include "mat.h"
#include <iostream>
#include <assert.h>

using namespace std;

void counterclockwise()
{
  // turn counter-clockwise as I look in the direction of the turning axis.
  Mat R=rotate(o,ez,var("%pi")/2.0);
  assert(eval(R[1][0]) > 0);
}

int main()
{
  counterclockwise();
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

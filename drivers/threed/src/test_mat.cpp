#include <iostream>
#include <math.h>
#include <assert.h>

#include "Vec3d.h"
#include "Mat3d.h"

using namespace std;

void test1()
{
  Mat3d m0=Mat3d::zero();
  Mat3d m1=Mat3d::identity();
  
  for (int r=0; r<3; ++r) {
    for (int c=0; c<4; ++c) {
      assert(m0(r,c)==0);
      assert(m1(r,c) == (r==c));
    }
  }
}

void rand(Mat3d &mat)
{
  for (int r=0; r<3; ++r) {
    for (int c=0; c<4; ++c) {
      mat(r,c)=(rand()%2001-1000)*0.01;
    }
  }
}

void rand(Vec3d &vec)
{
  for (int r=0; r<3; ++r) {
    vec(r)=(rand()%2001-1000)*0.01;
  }
}

void test2()
{
  Mat3d a;
  float A[4][4];
  Mat3d b;
  float B[4][4];

  rand(a);
  rand(b);
  for (int r=0; r<4; ++r) {
    for (int c=0; c<4; ++c) {
      A[r][c]=(r<3) ? a(r,c) : (r==c);
      B[r][c]=(r<3) ? b(r,c) : (r==c);
    }
  }


  float AB[4][4];
  for (int r=0; r<4; ++r) {
    for (int c=0; c<4; ++c) {
      AB[r][c]=0;
      for (int k=0; k<4; ++k) {
	AB[r][c]+=A[r][k]*B[k][c];
      }
    }
  }
  Mat3d ab=a*b;

  for (int r=0; r<4; ++r) {
    for (int c=0; c<4; ++c) {
      float abrc=(r < 3) ? ab(r,c) : (r==c);
      assert(fabs(AB[r][c]-abrc) < 1e-5);
    }
  }
}

void test3()
{
  Mat3d a;
  float A[4][4];
  Vec3d b;
  float B[4];

  rand(a);
  rand(b);
  for (int r=0; r<4; ++r) {
    for (int c=0; c<4; ++c) {
      A[r][c]=(r<3) ? a(r,c) : (r==c);
    }
    B[r]=(r<3) ? b(r) : 1;
  }


  float AB[4];
  Vec3d ab=a*b;

  for (int r=0; r<4; ++r) {
    AB[r]=0;
    for (int k=0; k<4; ++k) {
      AB[r]+=A[r][k]*B[k];
    }
  }

  for (int r=0; r<4; ++r) {
    float abr=(r < 3) ? ab(r) : 1;
    assert(fabs(AB[r]-abr) < 1e-5);
  }
}

void test4()
{
  Mat3d a;
  Mat3d b;
  Mat3d ab;

  rand(a);
  b=inverse(a);

  ab=a*b;

  for (int r=0; r<3; ++r) {
    for (int c=0; c<4; ++c) {
      assert(fabs(ab(r,c)-(r==c))<1e-4);
    }
  }
}

int main()
{
  test1();
  test2();
  test3();
  test4();
  cout << "ok" << endl;
  return 0;
}

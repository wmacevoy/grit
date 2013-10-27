#include "mat.h"

using namespace symbolic;
using namespace std;

bool is_const(const E &a) { 
  return isconstant(&*a);
}

bool is_var(const E &a) {
  return (!!a && typeid(*a) == typeid(Variable));
}

double eval(const E &a) {
  return evaluate(&*a);
}

std::string name(const E &a) {
  return (dynamic_cast<const Variable&>(*a)).name;
}

E operator+ (const E &a, const E &b)
{
  return simplify(E(new Sum(clone(a),clone(b))));
}

E operator- (const E &a, const E &b)
{
  return simplify(E(new Sum(clone(a),new Product(new Constant(-1.0),clone(b)))));
}

E operator- (const E &a)
{
  return simplify(E(new Product(new Constant(-1.0),clone(a))));
}

E operator* (const E &a, const E &b)
{
  return simplify(E(new Product(clone(a),clone(b))));
}

E operator/ (const E &a, const E &b)
{
  return simplify(E(new Product(clone(a), new Power(clone(b),new Constant(-1.0)))));
}

E pow(const E &a, const E&b) 
{
  return simplify(E(new Power(clone(a), clone(b))));
}

E log(const E &a)
{
  return simplify(E(new NaturalLog(clone(a))));
}

E cos(const E &a)
{
  return simplify(E(new Cos(clone(a))));
}

E sin(const E &a)
{
  return simplify(E(new Sin(clone(a))));
}

E var(const std::string &a)
{
  return E(new Variable(a));
}

E num(double a)
{
  return E(constant(a));
}

E simplify(const E &a)
{
  E sa=E(simplify(&*a));
  return sa;
}

E diff(const E &y, const E &x)
{
  return E(differentiate(&*y,name(x)));
}

symbolic::Expression* clone(const E &a)
{
  return (a ? a->clone() : 0);
}

Mat eye(int nr,int nc)
{
  Mat ans(nr,Vec(nc,num(0)));
  int n=nr < nc ? nr : nc;
  for (int i=0; i<n; ++i) {
    ans[i][i]=num(1);
  }
  return ans;
}

Mat zero(int nr, int nc)
{
  return Mat(nr,Vec(nc,num(0)));
}

int nr(const Mat &a)
{
  return int(a.size());
}

int nc(const Mat &a)
{
  return (a.size() > 0 ? a[0].size() : 0);
}

Mat operator*(const E &a, const Mat &b)
{
  int nrm = nr(b);
  int ncm = nc(b);
  Mat m(nrm,Vec(ncm,num(0)));

  for (int r=0; r<nrm; ++r) {
    for (int c=0; c<ncm; ++c) {
      m[r][c] = a*b[r][c];
    }
  }
  return m;
}

Mat operator*(const Mat &a, const Mat &b)
{
  int nr_a = nr(a);
  int nc_a = nc(a);
  int nr_b = nr(b);
  int nc_b = nc(b);

  if (nc_a != nr_b) {
    Expression::UnsupportedOperation error;
    error.message = "matrix inner dimensions must agree";
    throw error;
  }

  Mat ans(nr_a,Vec(nc_b,num(0)));

  int n = nc_a;

  for (int r=0; r < nr_a; ++r) {
    for (int c=0; c < nc_b; ++c) {
      E sum=num(0);
      for (int k=0; k<n; ++k) {
        sum = sum + a[r][k]*b[k][c];
      }
      ans[r][c]=sum;
    }
  }

  return ans;
}

Mat operator+ (const Mat &a, const Mat &b)
{
  int nr_a = nr(a);
  int nc_a = nc(a);
  int nr_b = nr(b);
  int nc_b = nc(b);

  if (nr_a != nr_b || nc_a != nc_b) {
    Expression::UnsupportedOperation error;
    error.message = "matrix dimensions must agree";
    throw error;
  }
  
  int nrm=nr_a;
  int ncm=nc_a;
  
  Mat m(nrm,Vec(ncm,num(0)));

  for (int r=0; r<nrm; ++r) {
    for (int c=0; c<ncm; ++c) {
      m[r][c] = a[r][c]+b[r][c];
    }
  }
  return m;
}

Mat operator- (const Mat &a, const Mat &b)
{
  return a+(num(-1)*b);
}


Mat transpose(const Mat &a)
{
  int nrm=nc(a);
  int ncm=nr(a);
  Mat m(nrm,Vec(ncm,num(0)));

  for (int r=0; r<nrm; ++r) {
    for (int c=0; c<ncm; ++c) {
      m[r][c]=E(a[c][r]->clone());
    }
  }
  return m;
}

Mat translate(const Vec &d)
{
  int nrm=d.size();
  int ncm=d.size();
  Mat m(nrm,Vec(ncm,num(0)));

  for (int r=0; r<nrm; ++r) {
    for (int c=0; c<ncm; ++c) {
      if (c == d.size()-1) {
        m[r][c]=d[r];
      } else {
        m[r][c]=num(r == c);
      }
    }
  }

  return m;
}

//
// rotate about line that contains p and goes in the unit direction n,
// the angle given by the cosine c and sine s.
//
Mat rotate(const Vec &p, const Vec &n, const E &cosT, const E &sinT)
{
  int nr=4;
  int nc=4;
  Mat ans(nr,Vec(nc,num(0)));

  // In this instance we normalize the direction vector.

  E a=p[0];
  E b=p[1];
  E c=p[2];

  E u=n[0];
  E v=n[1];
  E w=n[2];

  E u2=u*u;
  E uv=u*v;
  E uw=u*w;
  E v2=v*v;
  E vw=v*w;
  E w2=w*w;

  E oneMinusCosT = num(1)-c;

  Mat m(4,Vec(4,num(0)));

  m[0][0] = u2 + (v2 + w2) * cosT;
  m[0][1] = u*v * oneMinusCosT - w*sinT;
  m[0][2] = u*w * oneMinusCosT + v*sinT;
  m[0][3] = (a*(v2 + w2) - u*(b*v + c*w))*oneMinusCosT
                + (b*w - c*v)*sinT;

  m[1][0] = u*v * oneMinusCosT + w*sinT;
  m[1][1] = v2 + (u2 + w2) * cosT;
  m[1][2] = v*w * oneMinusCosT - u*sinT;
  m[1][3] = (b*(u2 + w2) - v*(a*u + c*w))*oneMinusCosT
                + (c*u - a*w)*sinT;

  m[2][0] = u*w * oneMinusCosT - v*sinT;
  m[2][1] = v*w * oneMinusCosT + u*sinT;
  m[2][2] = w2 + (u2 + v2) * cosT;
  m[2][3] = (c*(u2 + v2) - w*(a*u + b*v))*oneMinusCosT
                + (a*v - b*u)*sinT;

  m[3][0] = num(0);
  m[3][1] = num(0);
  m[3][2] = num(0);
  m[3][3] = num(1);

  return m;
}

Mat coords(const Vec &ex, const Vec &ey, const Vec &ez, const Vec &origin)
{
  int nr=4;
  int nc=4;
  Mat ans(nr,Vec(nc,num(0)));
  for (int r=0; r<nr; ++r) {
    for (int c=0; c<nc; ++c) {
      if (c == 0 && r < 3) {
        ans[r][c]=ex[r];
      } else if (c == 1 && r < 3) {
        ans[r][c]=ey[r];
      } else if (c == 2 && r < 3) {
        ans[r][c]=ez[r];
      } else if (c == 3 && r < 3) {
        ans[r][c]=origin[r];
      } else {
        ans[r][c]=num(r == c);
      }
    }
  }
  return simplify(ans);
}

Mat simplify(const Mat &a)
{
  int nrm=nr(a);
  int ncm=nc(a);

  Mat m(nrm,Vec(ncm,num(0)));
  for (int r=0; r<nrm; ++r) {
    for (int c=0; c<ncm; ++c) {
      m[r][c]=simplify(a[r][c]);
    }
  }

  return m;
}

Mat clone(const Mat &a)
{
  int nrm=nr(a);
  int ncm=nc(a);

  Mat m(nrm,Vec(ncm,num(0)));
  for (int r=0; r<nrm; ++r) {
    for (int c=0; c<ncm; ++c) {
      m[r][c]=E(a[r][c]->clone());
    }
  }

  return m;
}

Mat diff(const Mat &a, const E &x)
{
  int nrm=nr(a);
  int ncm=nc(a);
  string xname=name(x);

  Mat m(nrm,Vec(ncm,num(0)));
  for (int r=0; r<nrm; ++r) {
    for (int c=0; c<ncm; ++c) {
      m[r][c]=E(a[r][c]->differentiate(xname));
    }
  }

  return m;
}

Vec vec(const E &x, const E &y, const E &z)
{
  Vec ans(4,num(0));
  ans[0]=E(clone(x));
  ans[1]=E(clone(y));
  ans[2]=E(clone(z));
  ans[3]=num(1);
  return ans;
}

Mat column(const Vec &v)
{
  int n=int(v.size());
  int nrm=n;
  int ncm=1;
  Mat m(nrm,Vec(ncm,num(0)));
  for (int i=0; i<n; ++i) {
    m[i][0]=E(clone(v[i]));
  }

  return m;
}

Mat row(const Vec &v)
{
  int n=int(v.size());
  int nrm=1;
  int ncm=n;
  Mat m(nrm,Vec(ncm,num(0)));
  for (int i=0; i<n; ++i) {
    m[0][i]=E(clone(v[i]));
  }

  return m;
}

std::ostream& operator<<(std::ostream &out, const E &a)
{
  out << format_c_double(&*a);
  return out;
}

void define(std::ostream &out, const std::string &name, const Mat &A) {
  for (int r=0; r<nr(A); ++r) {
    for (int c=0; c<nc(A); ++c) {
      out << name << "[" << r << "][" << c << "]=" 
           << format_c_double(&*A[r][c]) << ";" << endl;
    }
  }
}


void declare(std::ostream &out, const std::string &name, const Mat &A) {
  out << "double " << name << "[" << nr(A) << "][" << nc(A) << "];" << endl;
}

void define(std::ostream &out, const std::string &name, const Vec &A) {
  for (size_t i=0; i != A.size(); ++i) {
    out << name << "[" << i << "]="
         << format_c_double(&*A[i]) << ";" << endl;
  }
}

void declare(std::ostream &out, const std::string &name, const Vec &A) {
  out << "double " << name << "[" << A.size() << "];" << endl;
}

const Vec o=vec(num(0),num(0),num(0));
const Vec ex=vec(num(1),num(0),num(0));
const Vec ey=vec(num(0),num(1),num(0));
const Vec ez=vec(num(0),num(0),num(1));

#pragma once

#include <tr1/memory>
#include "symbolic.hpp"
#include "formatter.hpp"

typedef std::tr1::shared_ptr < symbolic::Expression > E;
typedef std::vector < E > Vec;
typedef std::vector < Vec > Mat;

bool is_const(const E &a);
bool is_var(const E & a);
std::string name(const E &a);
double eval(const E &a);

E num(double a);
E operator+ (const E &a, const E &b);
E operator- (const E &a, const E &b);
E operator- (const E &a);
E operator* (const E &a, const E &b);
E operator/ (const E &a, const E &b);
E pow(const E &a, const E &b);
E log(const E &a);
E cos(const E &a);
E sin(const E &a);
E var(const std::string &a);
E num(double a);
E simplify(const E &a);
E diff(const E &y, const E &x);
symbolic::Expression *clone(const E &a);
Mat eye(int nr, int nc);
Mat zero(int nr, int nc);
int nr(const Mat &a);
int nc(const Mat &a);
Mat operator*(const E &a, const Mat &b);
Mat operator*(const Mat &a, const Mat &b);
Mat operator+(const Mat &a, const Mat &b);
Mat operator-(const Mat &a, const Mat &b);
Mat transpose(const Mat &a);
Mat translate(const Vec &d);
Mat rotate(const Vec &p, const Vec &n, const E &c, const E &s);
Mat coords(const Vec &ex, const Vec &ey, const Vec &ez, const Vec &origin);
Mat simplify(const Mat &a);
Mat clone(const Mat &a);
Vec vec(const E &x,const E &y, const E &z);

Mat column(const Vec &v);
Mat row(const Vec &v);

std::ostream& operator<<(std::ostream &out, const E &a);

void define(std::ostream &out, const std::string &name, const Mat &A);
void declare(std::ostream &out, const std::string &name, const Mat &A);
void define(std::ostream &out, const std::string &name, const Vec &A);
void declare(std::ostream &out, const std::string &name, const Mat &A);

extern const Vec o;
extern const Vec ex;
extern const Vec ey;
extern const Vec ez;
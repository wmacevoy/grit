%module threed
%include <std_string.i>
%include <std_shared_ptr.i>

%{
#define SWIG_FILE_WITH_INIT
#include "Vec3d.h"
#include "Mat3d.h"
#include "Arc3d.h"
%}

%rename(__add__) operator+;
%rename(__mul__) operator*;
%rename(__sub__) operator-(const Vec3d &a, const Vec3d &b);
%rename(__neg__) operator-(const Vec3d &a);
%rename(__sub__) operator-(const Mat3d &a, const Mat3d &b);
%rename(__neg__) operator-(const Mat3d &a);
%rename(__rshift__) operator>>;
%rename(__lshift__) operator<<;

%include "Vec3d.h"
%include "Mat3d.h"
%include "Arc3d.h"

%module safety
%include "std_string.i"
%include "std_shared_ptr.i"
%shared_ptr(Safety)
%{
#define SWIG_FILE_WITH_INIT
#include "CreateSafetyClient.h"
%}
%include "Safety.h"
%include "CreateSafetyClient.h"

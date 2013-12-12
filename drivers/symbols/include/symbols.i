%module symbols
%include "std_string.i"
%include "std_shared_ptr.i"
%{
#define SWIG_FILE_WITH_INIT
#include "Symbols.h"
%}

%include "Symbols.h"

prepreprocess -- package to make code generators from decorated source files.

to build

  make all

builds

  lib/libprepreprocess.a
  bin/prepreprocess
  bin/exampleprepreprocess

to test

  none available

to clean

  make clean

to use

  see exampleprepreprocess.cppp and the comments below

Prepreprocess can generate c++ source files that, when executed, can reproduce its input file in a manner similar to how php works to generate web pages.
There are a few simple rules:

1. a line starting with #! is quoted into the output verbatim (with the #! removed), so

#! for (int i=0; i<10; ++i) {
#! }

becomes

 for (int i=0; i<10; ++i) {
 }

2. a line starting with #' is converted into an output statement that, if executed, would reproduce the input verbatim (with the #' removed), so

#' O'Malley, the Alley Cat

becomes

std::cout << "O\'Malley, the Alley Cat" << std::endl;

Command-line options can change adjust the prefix, infix, and suffix portions of this.

3. a line starting with #", or one that does not start with #! or #', is converted into a statement that would reproduce the input except for $-excapes, so

$hello, $place; you are visitor number $(++counter).

becomes

std::cout << " " << hello << ", " << place << "; you are visitor number " << (++counter) << "." << std::endl;

A trailing $ suppresses the trailing endl, and nested () are allowed in the $() format.  Only simple identifiers are allowed in the $id format.



For example, with the input file (example.cppp):

#!#include <iostream>
#!#include <string>
#!
#!using namespace std;
#!int main()
#!{
#!  string greeting="hello";
#!

#include <iostream>
int main()
{
  cout << "$(greeting+" world")" << endl;
  return 0;
}
#! return 0;
#!}

running 

prepreprocess -out exampleprepreprocess.cpp -in exampleprepreprocess.cppp

produces

#line 1 "src/exampleprepreprocess.cppp"
#include <iostream>
#include <string>

using namespace std;
int main()
{
  string greeting="hello";
std::cout << std::endl;
std::cout << "#include <iostream>" << std::endl;
std::cout << "using namespace std;" << std::endl;
std::cout << std::endl;
std::cout << "int main()" << std::endl;
std::cout << "{" << std::endl;
std::cout << "  cout << \"" << (greeting+" world") << "\" << endl;" << std::endl;
std::cout << "  return 0;" << std::endl;
std::cout << "}" << std::endl;
std::cout << std::endl;
 return 0;
}

Compiling and running this produces

#include <iostream>
using namespace std;

int main()
{
  cout << "hello world" << endl;
  return 0;
}

Maintaining the prepreprocessor code is much more manageable than the 
generator's code, because of all the quoting escapes.  See 
cppcodegenterator.cppp for a production example.

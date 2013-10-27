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

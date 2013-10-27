#line 1 "src/gsolve.cppp"
#include <string>
#include <iostream>
#include <fstream>
#include "mat.h"

void gsolve(const std::string &name, const Vec &equations, 
           const Vec &variables, const Vec &parameters) {
 std::ofstream out((std::string("ik_")+name+".ini").c_str());
out << "#" << std::endl;
out << "# sample configuration file." << std::endl;
out << "#" << std::endl;
out << "[flags]" << std::endl;
out << "prefix=\"ik_" << (name) << "\" # prefix prepended to function and argument names" << std::endl;
out << "file=\"ik_" << (name) << "\"    # path to output file (suffixes added by generators)" << std::endl;
out << "precision=\"single\" # can be \"double\" or \"single\"" << std::endl;
out << "namespace=\"\" # used by c++ generator " << std::endl;
out << "main=\"false\" # if true, then a main() is defined by the c++ generator" << std::endl;
out << "solver=\"damped_newton\"" << std::endl;
out << "[globals] # values shared among all cells" << std::endl;
out << "t" << std::endl;
out << "epsilon" << std::endl;
out << "steps" << std::endl;
out << "[parameters] # values specific to each cell" << std::endl;
 for (Vec::const_iterator i=parameters.begin(); i != parameters.end(); ++i) {
out << (*i) << std::endl;
 }
out << "residual" << std::endl;
out << "[variables]" << std::endl;
 for (Vec::const_iterator i=variables.begin(); i != variables.end(); ++i) {
out << (*i) << "_" << std::endl;
 }
out << "[initializers] # initial guess for solution of variables in terms of parameters/globals" << std::endl;
 for (Vec::const_iterator i=variables.begin(); i != variables.end(); ++i) {
out << (*i) << "_=" << (*i) << std::endl;
 }
out << "[equations] # system of equations variables should be a root of" << std::endl;
 int count = 0; 
 for (Vec::const_iterator i=equations.begin(); i != equations.end(); ++i) {
out << "residual" << count << "=" << (*i) << std::endl;
  ++count;
 }
out << "[updates] # how parameters are updated in terms of temp variables" << std::endl;
 for (Vec::const_iterator i=variables.begin(); i != variables.end(); ++i) {
out << (*i) << "=" << (*i) << "_" << std::endl;
 }
out << "[tests]" << std::endl;
out << "#" << std::endl;
out << "[end]" << std::endl;
} // gsolve

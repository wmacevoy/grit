#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdexcept>

#include "Configure.h"
#include "CSVRead.h"
#include "CSVSplit.h"
#include "split.h"

using namespace std;

void Configure::load(const std::string &file)
{
  string name;
  if (file.length() > 0 && file[0] == '/' && !!ifstream(file.c_str())) {
    name = file;
  } else {
    for (Paths::iterator p = paths.begin(); p != paths.end(); ++p) {
      name = *p;
      if (name.length() > 0 && name[name.length() - 1] != '/') {
	name.append("/");
      }
      name.append(file);
      if (!!ifstream(name.c_str())) break;
      name = "";
    }
  }
  
  vector < vector < string > > table;
  if (CSVRead(name,"name,value",table)) {
    for (size_t row=0; row < table.size(); ++row) {
      values[table[row][0]] = table[row][1];
    }
  }
}

void Configure::path(const std::string &spath)
{
  split(spath,paths,':',false);
}

void Configure::set(const std::string &name, const std::string &value)
{
  values[name]=value;
}

void Configure::args(const std::string &prefix, char **argv)
{
  for (char **argp = argv; *argp != 0; ++argp) {
    string arg = *argp;
    if (arg.length() > 2 && (arg[0] == '-' && arg[1] == '-')) {
      string name = arg.substr(2);
      char *op = *(argp+1);
      string value;
      if (op == 0 || (strlen(op) >= 2 && (op[0]=='-' && op[1] == '-'))) {
	value = "true";
      } else {
	value = op;
	++argp;
      }

      string fullname;
      if (name[0]=='.') {
	fullname = prefix;
	if (fullname.length() > 0 && fullname[fullname.length()-1] == '.') {
	  fullname.resize(fullname.length()-1);
	}
	fullname.append(name);
      } else {
	fullname = name;
      }

      if (name == "configure") {
	load(op);
      } else {
	cout << fullname << "->" << value << endl;
	values[fullname]=value;
      }
    }
  }
}

std::string Configure::substitute(const std::string &word_) const
{
  std::string word=word_;
  bool matches = true;
  size_t i = 0;
  while (matches) {
    matches = false;
    for (;;) {
      i=word.find('$',i);
      if (i != std::string::npos) {
	++i;
	if (i < word.length() && word[i]=='{') {
	  ++i;
	  size_t j=i+1;
	  while (j < word.length() && (word[j]=='.'||word[j]=='_'||isalpha(word[j])||isdigit(word[j]))) { ++j; }
	  if (j < word.length() && word[j] == '}') {
	    if (j > i+1) {
	      string parameter = word.substr(i,j-i);
	      map < string , string > :: const_iterator k = 
		values.find(parameter);
	      if (k != values.end()) {
		string pre=word.substr(0,i-2);
		string post=word.substr(j+1);
		string newWord = pre + k->second + post;
		word=newWord;
		i += pre.length() + k->second.length();
		matches = true;
	      }
	    }
	  }
	}
      }
      if ((i == string::npos || i >= word.length())) {
	if (matches) {
	  i = 0;
	  matches = false;
	}  else {
	  break;
	}
      }
    }
  }
  return word;
}

std::string Configure::str(const std::string &name) const
{
  map < string , string > :: const_iterator i = values.find(name);
  if (i == values.end()) {
    throw out_of_range(name);
  }
  return substitute(i->second);
}

std::string Configure::str(const std::string &name, const std::string &def) const
{
  map < string , string > :: const_iterator i = values.find(name);
  return substitute((i != values.end()) ? i->second : def);
}

double Configure::num(const std::string &name) const
{
  return atof(str(name).c_str());
}

double Configure::num(const std::string &name, double def) const
{
  map < string , string > :: const_iterator i = values.find(name);
  return (i != values.end()) ? atof(substitute(i->second).c_str()) : def;
}

bool Configure::flag(const std::string &name) const
{
  return str(name) == "true";
}

bool Configure::flag(const std::string &name, bool def) const
{
  map < string , string > :: const_iterator i = values.find(name);
  return (i != values.end()) ? substitute(i->second) == "true" : def;
}

std::vector<std::string> Configure::list(const std::string &name) const
{
  vector<string> ans;
  CSVSplit(str(name),ans);
  return ans;
}

void Configure::show() const { show(cout); }

void Configure::show(std::ostream &out) const 
{
  for (Values::const_iterator i = values.begin(); i != values.end(); ++i) {
    out << i->first << "='" << i->second << "'" << endl;
  }
}

void Configure::servos()
{
  servos(str("servos.map"));
}

void Configure::servos(const std::string &file)
{
  servoNameMap.clear();
  servoIdMap.clear();
  servoColumnMap.clear();
  servoTable.clear();
  string header = "name,id,device,scale,offset,torque,speed";
  vector<string> cols;
  split(header,cols);
  for (size_t i=0; i<cols.size(); ++i) {
    servoColumnMap[cols[i]] = i;
  }
  CSVRead(file,header,servoTable);
  for (size_t i=0; i<servoTable.size(); ++i) {
    servoNameMap[servoTable[i][0]]=i;
    servoIdMap[atoi(servoTable[i][1].c_str())]=i;
  }
  if (servoNameMap.empty()) {
    throw invalid_argument(file);
  }
}

std::string Configure::servo(int id, const std::string &parameter) const
{
  ServoIdMap::const_iterator i = servoIdMap.find(id);
  if (i == servoIdMap.end()) {
    ostringstream oss;
    oss << id;
    throw out_of_range(oss.str());
  }
  ServoColumnMap::const_iterator j = servoColumnMap.find(parameter);
  if (j == servoColumnMap.end()) {
    throw out_of_range(parameter);
  }
  return substitute(servoTable[i->second][j->second]);
}

std::string Configure::servo(const std::string &name, const std::string &parameter) const
{
  ServoNameMap::const_iterator i = servoNameMap.find(name);
  if (i == servoNameMap.end()) {
    return substitute(parameter);
  }
  ServoColumnMap::const_iterator j = servoColumnMap.find(parameter);
  if (j == servoColumnMap.end()) {
    return substitute(parameter);
  }
  return substitute(servoTable[i->second][j->second]);
}

bool Configure::find(const std::string &name) const
{
  return values.find(name) != values.end();
}

std::set < std::string > Configure::servoNames() const
{
  std::set < std::string > ans;
  for (ServoNameMap::const_iterator i=servoNameMap.begin(); i!=servoNameMap.end(); ++i) {
    ans.insert(i->first);
  }
  return ans;
}


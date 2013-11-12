#pragma once

#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>

class Configure
{
  typedef std::vector < std::string > Paths;
  Paths paths;

  typedef std::map < std::string , std::string > Values;

  typedef std::vector < std::vector < std::string > > ServoTable;
  typedef std::map < std::string , int > ServoNameMap;
  typedef std::map < std::string , int > ServoColumnMap;
  typedef std::map < int , int > ServoIdMap;
  Values values;
  ServoTable servoTable;
  ServoIdMap servoIdMap;
  ServoNameMap servoNameMap;
  ServoColumnMap servoColumnMap;

 public:
  void path(const std::string &spath);
  void load(const std::string &file);
  std::string pathfind(const std::string &file);
  void set(const std::string &name, const std::string &value);
  void args(const std::string &prefix, char **argv);
  std::string str(const std::string &name) const;
  std::string str(const std::string &name, const std::string &def) const;
  double num(const std::string &name) const;
  double num(const std::string &name, double def) const;
  bool flag(const std::string &name) const;
  bool flag(const std::string &name, bool def) const;
  std::vector<std::string> list(const std::string &name) const;
  void show() const;
  void show(std::ostream &out) const;

  bool find(const std::string &name) const;

  void servos(const std::string &file);
  void servos();
  std::set < std::string > servoNames() const;
  std::string substitute(const std::string &word) const;
  std::string servo(const std::string &name,const std::string &parameter) const;
  std::string servo(int id,const std::string &parameter) const;
};

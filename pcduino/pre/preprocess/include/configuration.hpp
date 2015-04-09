#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <map>
#include <list>
#include <iostream>

namespace preprocess {

class Configuration
{
public:
  class Case
  {
  public:
    std::string name;
    std::list < std::pair < std::string , double > > setup;
  };

  std::map < std::string , std::string > flags;
  std::list < std::string > globals;
  std::list < std::string > parameters;
  std::list < std::string > variables;
  std::list < std::pair < std::string , std::string > > initializers;
  std::list < std::pair < std::string , std::string > > equations;
  std::list < std::pair < std::string , std::string > > updates;

  std::list < Case > tests;
  std::list < Case > runs;
};

#define CONFIGURATION_FOREACH_FLAG(CONFIGURATION,INDEX) for (std::map < std::string , std::string > :: iterator INDEX = (CONFIGURATION).flags.begin(); (INDEX) != ((CONFIGURATION).flags.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_GLOBAL(CONFIGURATION,INDEX) for (std::list < std::string > :: iterator INDEX = (CONFIGURATION).globals.begin(); (INDEX) != ((CONFIGURATION).globals.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_PARAMETER(CONFIGURATION,INDEX) for (std::list < std::string > :: iterator INDEX = (CONFIGURATION).parameters.begin(); (INDEX) != ((CONFIGURATION).parameters.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_VARIABLE(CONFIGURATION,INDEX) for (std::list < std::string > :: iterator INDEX = (CONFIGURATION).variables.begin(); (INDEX) != ((CONFIGURATION).variables.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_INITIALIZER(CONFIGURATION,INDEX) for (std::list < std::pair < std::string , std::string > > :: iterator INDEX = (CONFIGURATION).initializers.begin(); (INDEX) != ((CONFIGURATION).initializers.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_EQUATION(CONFIGURATION,INDEX) for (std::list < std::pair < std::string , std::string > > :: iterator INDEX = (CONFIGURATION).equations.begin(); (INDEX) != ((CONFIGURATION).equations.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_UPDATE(CONFIGURATION,INDEX) for (std::list < std::pair < std::string , std::string > > :: iterator INDEX = (CONFIGURATION).updates.begin(); (INDEX) != ((CONFIGURATION).updates.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_TEST(CONFIGURATION,INDEX) for (std::list < Configuration::Case > :: iterator INDEX = (CONFIGURATION).tests.begin(); (INDEX) != ((CONFIGURATION).tests.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_RUN(CONFIGURATION,INDEX) for (std::list < Configuration::Case > :: iterator INDEX = (CONFIGURATION).runs.begin(); (INDEX) != ((CONFIGURATION).runs.end()); ++(INDEX))


#define CONFIGURATION_FOREACH_RUN(CONFIGURATION,INDEX) for (std::list < Configuration::Case > :: iterator INDEX = (CONFIGURATION).runs.begin(); (INDEX) != ((CONFIGURATION).runs.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_CASE_SETUP(CASE,INDEX) for (std::list < std::pair < std::string , double > > :: iterator INDEX = (CASE).setup.begin(); (INDEX) != (CASE).setup.end(); ++(INDEX))

#define CONFIGURATION_FOREACH_CONST_FLAG(CONFIGURATION,INDEX) for (std::map < std::string , std::string > :: const_iterator INDEX = (CONFIGURATION).flags.begin(); (INDEX) != ((CONFIGURATION).flags.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_CONST_GLOBAL(CONFIGURATION,INDEX) for (std::list < std::string > :: const_iterator INDEX = (CONFIGURATION).globals.begin(); (INDEX) != ((CONFIGURATION).globals.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_CONST_PARAMETER(CONFIGURATION,INDEX) for (std::list < std::string > :: const_iterator INDEX = (CONFIGURATION).parameters.begin(); (INDEX) != ((CONFIGURATION).parameters.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_CONST_VARIABLE(CONFIGURATION,INDEX) for (std::list < std::string > :: const_iterator INDEX = (CONFIGURATION).variables.begin(); (INDEX) != ((CONFIGURATION).variables.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_CONST_INITIALIZER(CONFIGURATION,INDEX) for (std::list < std::pair < std::string , std::string > > :: const_iterator INDEX = (CONFIGURATION).initializers.begin(); (INDEX) != ((CONFIGURATION).initializers.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_CONST_EQUATION(CONFIGURATION,INDEX) for (std::list < std::pair < std::string , std::string > > :: const_iterator INDEX = (CONFIGURATION).equations.begin(); (INDEX) != ((CONFIGURATION).equations.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_CONST_UPDATE(CONFIGURATION,INDEX) for (std::list < std::pair < std::string , std::string > > :: const_iterator INDEX = (CONFIGURATION).updates.begin(); (INDEX) != ((CONFIGURATION).updates.end()); ++(INDEX))


#define CONFIGURATION_FOREACH_CONST_TEST(CONFIGURATION,INDEX) for (std::list < Configuration::Case > :: const_iterator INDEX = (CONFIGURATION).tests.begin(); (INDEX) != ((CONFIGURATION).tests.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_CONST_RUN(CONFIGURATION,INDEX) for (std::list < Configuration::Case > :: const_iterator INDEX = (CONFIGURATION).runs.begin(); (INDEX) != ((CONFIGURATION).runs.end()); ++(INDEX))

#define CONFIGURATION_FOREACH_CONST_CASE_SETUP(CASE,INDEX) for (std::list < std::pair < std::string , double > > :: const_iterator INDEX = (CASE).setup.begin(); (INDEX) != (CASE).setup.end(); ++(INDEX))

bool configuration_parse(const std::string &file, Configuration &configuration);

std::ostream& operator<< (std::ostream &out, const Configuration &configuration);

}

#endif

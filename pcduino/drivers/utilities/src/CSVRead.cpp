#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unistd.h>

#include "CSVRead.h"
#include "CSVSplit.h"

using namespace std;

bool CSVRead(const std::string &file, 
	     const std::string &heading_str, 
	     std::vector < std::vector < std::string > > &table)
{
  table.clear();
  ifstream in(file);

  if (!in) { 
	  char buffer[1024];
	  cout << "Could not open file " << endl;
	  cout << "[" << file << "]"<< endl; 
	  cout << "Current path is "<< getcwd(buffer,1024) << endl; 
	  cout.flush();
	  return false; 
  }
  vector<string> heading_vec;
  vector<string> line_vec;
  
  map<string,int> heading_map;
  set<string> line_set;

  vector<int> column_map;

  CSVSplit(heading_str,heading_vec);

  for (size_t i=0; i<heading_vec.size(); ++i) {
    heading_map[heading_vec[i]]=i;
  }

  string line;
  bool match = false;

  for (;;) {
    if (!getline(in,line))  {
      return false; 
    }
    CSVSplit(line,line_vec);
    line_set.clear();
    for (size_t i=0; i<line_vec.size(); ++i) {
      line_set.insert(line_vec[i]);
    }
    
    match = true;
    for (size_t i=0; i != heading_vec.size(); ++i) {
      if (line_set.find(heading_vec[i].c_str()) == line_set.end()) {
	match = false; 
	break;
      }
    }
    if (match) break;
  }

  if (!match) {
    cout.flush();
    return false;
  } else {
    cout.flush();
  }
  

  column_map.resize(heading_vec.size());
  int min_index = -1;
  for (size_t i=0; i != line_vec.size(); ++i) {
    map<string,int>::iterator j=heading_map.find(line_vec[i]);
    if (j != heading_map.end()) {
      column_map[j->second]=i;
      if (min_index == -1) min_index = i;
    }
  }

  for (;;) {
    if (!getline(in,line,'\n')) break;
    CSVSplit(line,line_vec);
    if ((ssize_t)line_vec.size() <= min_index || line_vec[min_index] == "") break;
    table.push_back(vector<string>());
    vector<string> &row=*table.rbegin();
    row.resize(heading_vec.size());

    for (size_t i=0; i<heading_vec.size(); ++i) {
      row[i]=line_vec[column_map[i]];
    }
  }
  if (table.size()==0) {
    cout.flush();
  }
  return table.size() > 0;
}

bool CSVRead(const std::string &file, 
	     const std::string &heading, 
	     std::vector < std::vector < double > > &table)
{
  std::vector < std::vector < std::string > > stable;
  table.clear();
  if (CSVRead(file,heading,stable)) {
    table.resize(stable.size());
    for (size_t i=0; i<stable.size(); ++i) {
      std::vector < std::string > & srow = stable[i];
      table[i].resize(srow.size());
      std::vector < double > & row = table[i];
      for (size_t j=0; j<srow.size(); ++j) {
	row[j] = atof(srow[j].c_str());
      }
    }
    return true;
  } 
  return false;
}
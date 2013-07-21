#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

#include "CSVRead.h"


using namespace std;

int main()
{
  vector < vector < double > > table;

  string heading="a,b,c";
  CSVRead("example.csv","a,b,c",table);

  cout << "a,b,c" << endl;
  for (size_t i=0; i<table.size(); ++i) {
    for (size_t j=0; j<table[i].size(); ++j) {
      cout << ((j > 0) ? "," : "") << table[i][j];
    }
    cout << endl;
  }
}

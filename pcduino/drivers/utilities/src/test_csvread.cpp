#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <assert.h>

#include "CSVRead.h"


using namespace std;

int main()
{
  vector < vector < double > > table;

  string heading="a,b,c";
  CSVRead("example.csv","a,b,c",table);
  assert(table.size() == 2);
  for (size_t i=0; i<table.size(); ++i) {
    assert(table[i].size() == 3);
    for (size_t j=0; j<table[i].size(); ++j) {
      assert(table[i][j]==10*i+j);
    }
  }
  cout << "ok" << endl;
  return 0;
}

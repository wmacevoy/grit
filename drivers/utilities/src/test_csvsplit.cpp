#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <assert.h>

#include "CSVSplit.h"

using namespace std;

const char *line1 = "";
const char *part1[] = { 0 };

const char *line2 = "a";
const char *part2[] = { "a", 0 };

const char *line3 = "ab";
const char *part3[] = { "ab", 0 };

const char *line4 = "abc";
const char *part4[] = { "abc", 0 };

const char *line5 = "a,c";
const char *part5[] = { "a","c", 0 };

const char *line6 = ",\"\"";
const char *part6[] = { "","", 0 };

const char *line7 = ",\"\",";
const char *part7[] = { "","", "", 0 };

const char *line8 = ",\"a\"\"b\",";
const char *part8[] = { "","a\"b", "", 0 };

const char *line9 = "a,\"x\"";
const char *part9[] = { "a","x", 0 };

typedef struct {
  const char *line;
  const char **parts;
} Test;

const Test tests[] = {
  { line1, part1 },
  { line2, part2 },
  { line3, part3 },
  { line4, part4 },
  { line5, part5 },
  { line6, part6 },
  { line7, part7 },
  { line8, part8 },
  { line9, part9 }
};

void test(const Test &test)
{
  string line = test.line;
  vector < string > parts;
  CSVSplit(line,parts);
  for (size_t i=0; test.parts[i] != 0; ++i) {

    assert(i < parts.size() && parts[i] == test.parts[i]);
  }
  //  cout << "test " << test.line << " ok" << endl;
}

int main()
{
  for (size_t i=0; i<sizeof(tests) / sizeof(Test); ++i) {
    test(tests[i]);
  }
  cout << "ok" << endl;
  return 0;
}


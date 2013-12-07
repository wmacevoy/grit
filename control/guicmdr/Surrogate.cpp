#include <iostream>
#include <string>

using namespace std;

int main(int argc, char ** argv) {
  string text;
  while (!cin.eof()) {
    cin >> text;
    cout << "Received: "<< text << endl;
    if (text=="exit") break;
  }
}

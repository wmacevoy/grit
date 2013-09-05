#include <HokuyoProviderRequest.hpp>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

bool wantsToQuit(int &nScans) {
	nScans = 0;
	std::cout << "Type \"q\" or \"quit\" to quit, Otherwise, type the number of scans you want to receive.\n";
	std::string input;
	std::cin >> input;
	bool wantsToQuit = input.compare("q") == 0 || input.compare("quit") == 0;
	if(wantsToQuit)
		return true;
	istringstream(input) >> nScans;
	if(nScans < 0)
		nScans = 0;
	return false;
}

int main( int argc, const char* argv[] )
{	
	int nScans = 0;
	while(!wantsToQuit(nScans) && nScans > 0){
		//std::cout << "You are going to request more data\n";
		HokuyoData data = HokuyoProviderRequest::GetData("tcp://192.168.2.100", nScans);
		cout << "Data arrived." << endl;
		//	data.printToStdOut();
	}
}

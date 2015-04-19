#pragma once

#include <string>
#include <vector>

void split(const std::string &together, 
	   std::vector<std::string> &parts, char sep=',', bool trim=true);


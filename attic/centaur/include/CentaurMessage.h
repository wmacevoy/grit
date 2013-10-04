#pragma once

#include <vector>

// ****************************************************************************
// CentaurMessage

class CentaurMessage {

protected:



public:

	void save(std::vector<unsigned char> data);
	void load(const unsigned char * pData, size_t sizeData);

	CentaurMessage();
	virtual ~CentaurMessage(){}
};
